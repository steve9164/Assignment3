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

static const char *vertexShaderSourceCore =
    "#version 330\n"
    "in vec4 vertex;\n"
    "in vec3 position;\n"
    "in float scale;\n"
    "in vec4 vertexColor;\n"
    "out vec4 fragmentColor;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "void main() {\n"
    "    gl_Position = projMatrix * viewMatrix * vec4(scale*vertex.xyz + position, 1);\n"
    "    fragmentColor = vertexColor;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 330\n"
    "in vec4 fragmentColor;\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = fragmentColor;\n"
    "}\n";

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
    static_assert (sizeof(struct BodyRenderDetails) == 8*sizeof(float), "Renderer3D::BodyRenderDetails size is not correct");

    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, sizeof(struct BodyRenderDetails));

    glVertexAttribDivisor(m_program->attributeLocation("position"), 1);

    m_program->enableAttributeArray("scale");
    m_program->setAttributeBuffer("scale", GL_FLOAT, 3, 1, sizeof(struct BodyRenderDetails));
    glVertexAttribDivisor(m_program->attributeLocation("scale"), 1);

    m_program->enableAttributeArray("vertexColor");
    m_program->setAttributeBuffer("vertexColor", GL_FLOAT, 4, 4, sizeof(struct BodyRenderDetails));
    glVertexAttribDivisor(m_program->attributeLocation("vertexColor"), 1);




    m_program->release();
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

    //qDebug() << pos << radius << QVector4D(r,g,b,a);

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
    m_view.translate(-m_cameraVelocity);

    m_proj.setToIdentity();
    m_proj.perspective(45.0f, (GLfloat)m_widget->width() / m_widget->height(), 0.01f, 100.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Bind m_vao and release when vaoBinder goes out of scope
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Test m_bodyRenderDetailsAccumulator.data():

    float* data = (float*)(void*)m_bodyRenderDetailsAccumulator.data();
    qDebug() << "Printing m_bodyRenderDetailsAccumulator:";
    while (data < (float*)(m_bodyRenderDetailsAccumulator.data()+m_bodyRenderDetailsAccumulator.size()))
    {
        qDebug() << *data << *(data+1) << *(data+2) << *(data+3) << *(data+4) << *(data+5) << *(data+6) << *(data+7);
        data += 8;
    }

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
