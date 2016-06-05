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
    "uniform mat4 projMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "void main() {\n"
    "    gl_Position = projMatrix * viewMatrix * vec4(scale*vertex.xyz + position, 1);\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 330\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = vec4(0, 1.0, 1.0, 1.0);\n"
    "}\n";

Renderer3D::Renderer3D(QOpenGLWidget* widget)
    : m_view(),
      m_cameraVelocity(),
      m_program(new QOpenGLShaderProgram),
      m_cubeIndices(QOpenGLBuffer::IndexBuffer),
      m_cubeLocations()
{
    m_view.setToIdentity();
    m_view.translate(0, 0, -10);

    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->bindAttributeLocation("vertex", 0);

    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_viewMatrixLoc = m_program->uniformLocation("viewMatrix");

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

    std::array<QVector3D, 4> locations{{{2.0f, 2.0f, 0.0f}, {2.0f, -2.0f, 0.0f}, {-2.0f, 2.0f, 0.0f}, {-2.0f, -2.0f, 0.0f}}};

    m_cubeLocations.create();
    m_cubeLocations.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_cubeLocations.bind();
    m_cubeLocations.allocate(locations.data(), locations.size() * sizeof(QVector3D));

    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, 3, sizeof(QVector3D));

    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    f->glVertexAttribDivisor(m_program->attributeLocation("position"), 1);

    std::array<float, 4> sizes{{0.5f, 0.25f, 1.25f, 1.5f}};

    m_cubeSizes.create();
    m_cubeSizes.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_cubeSizes.bind();
    m_cubeSizes.allocate(sizes.data(), sizes.size() * sizeof(float));

    m_program->enableAttributeArray("scale");
    m_program->setAttributeBuffer("scale", GL_FLOAT, 0, 1, sizeof(float));


    f->glVertexAttribDivisor(m_program->attributeLocation("scale"), 1);

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
//    m_cubeVertices.bind();
//    m_cubeIndices.bind();
    m_widget = widget;
    m_view.translate(-m_cameraVelocity);

//    m_view.setToIdentity();

//    m_view.translate(0,0,-6);


    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(widget->width()) / widget->height(), 0.01f, 100.0f);

    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);

    // Bind m_vao and release when vaoBinder goes out of scope
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_viewMatrixLoc, m_view);


    // Draw cube geometry using indices from VBO 1
    f->glDrawElementsInstanced(GL_TRIANGLE_STRIP, 16, GL_UNSIGNED_SHORT, 0, 4);

    m_program->release();
}

void Renderer3D::drawBody(const UniverseBody& body)
{
}

void Renderer3D::drawZodiac(const Zodiac& zodiac)
{
}

void Renderer3D::drawLabel(const UniverseBody& body)
{
}


void Renderer3D::finishRender()
{
}
