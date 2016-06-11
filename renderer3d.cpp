#include "renderer3d.h"
#include "config.h"
#include "renderer3deventhandlers.h"

#include <QGradient>
#include <QWidget>
#include <QDebug>
#include <algorithm>
#include <array>
#include <QDebug>
#include <QSurfaceFormat>
#include <QOpenGLFunctions_3_3_Core>
#include <QQuaternion>

// Use C++11 raw string literals for GLSL shader source code

static const char *vertexShaderSourceCore = R"(
    #version 330
    in vec4 vertex;
    in vec3 position;
    in float scale;
    in vec4 vertexColor;
    out vec4 fragmentColor;
    uniform mat4 projMatrix;
    uniform mat4 viewMatrix;
    void main() {
        gl_Position = projMatrix * viewMatrix * vec4(scale*vertex.xyz + position, 1);
        fragmentColor = vertexColor;
    }
)";

static const char *fragmentShaderSourceCore = R"(
    #version 330
    in vec4 fragmentColor;
    out vec4 color;
    void main() {
       color = fragmentColor;
    }
)";

struct Renderer3D::BodyRenderDetails
{
    QVector3D position;
    float scale;
    QVector4D color;
};

Renderer3D::Renderer3D(QOpenGLWidget* widget)
    : m_view(),
      m_cameraVelocity(),
      m_program(),
      m_cubeIndices(QOpenGLBuffer::IndexBuffer),
      m_widget(widget)
{
    qDebug() << "Test";
    qDebug() << initializeOpenGLFunctions();
    m_program = new QOpenGLShaderProgram();
    m_view.setToIdentity();
    //m_view.translate(0.0f, 0.0f, -3.0f);

    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->bindAttributeLocation("vertex", 0);

    m_program->link();

    m_program->bind();

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    std::array<QVector3D, 8> vertices{{{-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,1.0f}, {-1.0f,1.0f,-1.0f}, {-1.0f,1.0f,1.0f}, {1.0f,-1.0f,-1.0f}, {1.0f,-1.0f,1.0f}, {1.0f,1.0f,-1.0f}, {1.0f,1.0f,1.0f}}};
    std::array<GLushort, 16> vertexIndices{{2,0,6,4,5,0,1,2,3,6,7,5,3,1}};

    // Setup our vertex buffer object.
    m_cubeVertices.create();
    m_cubeVertices.bind();
    m_cubeVertices.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

    m_cubeIndices.create();
    m_cubeIndices.bind();
    m_cubeIndices.allocate(vertexIndices.data(), vertexIndices.size() * sizeof(GLushort));

    // Tell OpenGL programmable pipeline how to locate vertex position data
    m_program->enableAttributeArray("vertex");
    m_program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, sizeof(QVector3D));

//    std::array<QVector3D, 4> locations{{{2.0f, 2.0f, 0.0f}, {2.0f, -2.0f, 0.0f}, {-2.0f, 2.0f, 0.0f}, {-2.0f, -2.0f, 0.0f}}};

    m_cubeInstanceData.create();
    m_cubeInstanceData.bind(); // All of the attributes below are in this vbo

    // Compile time assertion that struct BodyRenderDetails is laid out as assumed
    static_assert(sizeof(struct BodyRenderDetails) == 8*sizeof(float), "Renderer3D::BodyRenderDetails layout is not correct. Should be 3 floats | 1 float | 4 floats = 8 floats overall");

    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, sizeof(struct BodyRenderDetails));

    glVertexAttribDivisor(m_program->attributeLocation("position"), 1);

    m_program->enableAttributeArray("scale");
    m_program->setAttributeBuffer("scale", GL_FLOAT, 3*sizeof(float), 1, sizeof(struct BodyRenderDetails));
    glVertexAttribDivisor(m_program->attributeLocation("scale"), 1);

    m_program->enableAttributeArray("vertexColor");
    m_program->setAttributeBuffer("vertexColor", GL_FLOAT, 4*sizeof(float), 4, sizeof(struct BodyRenderDetails));
    glVertexAttribDivisor(m_program->attributeLocation("vertexColor"), 1);

    m_program->release();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

Renderer3D::~Renderer3D()
{
    m_cubeVertices.destroy();
    m_cubeIndices.destroy();
}

std::shared_ptr<EventHandler> Renderer3D::buildEventChain()
{
    std::shared_ptr<EventHandler> handler(new KeyEventHandler(*this));
    handler->chain(std::shared_ptr<EventHandler>(new MouseWheelEventHandler(*this)));
    return handler;
}

void Renderer3D::autoAdjustCamera(std::pair<QVector3D, QVector3D> boundingBox)
{
    m_view.setToIdentity();
}

void Renderer3D::startRender(QWidget* widget)
{
    // Clear previous frame's Body details
    m_bodyRenderDetailsAccumulator.clear();
}

void Renderer3D::drawBody(const UniverseBody& body)
{
    Config* config = Config::getInstance();

    //get scaled position and radius
    QVector3D pos = body.getPosition() / config->getDistanceScale();
    double radius = body.getRadius() / config->getRadiusScale();

    if(config->getUseLogRadius())
    {
        radius = std::log(body.getRadius() / config->getLogPointRadius());
    }
    double r,g,b,a;
    body.getColor().getRgbF(&r,&g,&b,&a);

//    qDebug() << pos << radius << QVector4D(r,g,b,a);
//    qDebug() << "Radius:" << body.getRadius() << config->getRadiusScale() << config->getUseLogRadius();

    m_bodyRenderDetailsAccumulator.push_back({pos, static_cast<float>(radius), QVector4D(r,g,b,a)});
}

void Renderer3D::drawZodiac(const Zodiac& zodiac)
{
}

void Renderer3D::drawLabel(const UniverseBody& body)
{
}


void Renderer3D::finishRender()
{
//    m_view.translate(-m_cameraVelocity);

    // V[n+1] = Rotation[inv] * V[n]
    // Qt can't invert a matrix, so invert a quaternion
    QMatrix4x4 rot;
    rot.rotate(1.0f, QVector3D(-m_cameraVelocity.y(), m_cameraVelocity.x(), 0.0f));
    m_view = rot * m_view;

    m_proj.setToIdentity();
    m_proj.perspective(45.0f, (GLfloat)m_widget->width() / m_widget->height(), 0.01f, 1000.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind m_vao and release when vaoBinder goes out of scope
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Test m_bodyRenderDetailsAccumulator.data():

    m_cubeInstanceData.bind();
    m_cubeInstanceData.allocate(m_bodyRenderDetailsAccumulator.data(), m_bodyRenderDetailsAccumulator.size() * sizeof(struct BodyRenderDetails));

    m_program->bind();
    m_program->setUniformValue("projMatrix", m_proj);
    m_program->setUniformValue("viewMatrix", m_view);

    static unsigned int counter = 0;

    //qDebug() << "Rendering scene:" << counter++;

    // Draw cube geometry using indices from VBO 1
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 16, GL_UNSIGNED_SHORT, 0, m_bodyRenderDetailsAccumulator.size());

    m_program->release();
}
