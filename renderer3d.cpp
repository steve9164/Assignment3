#include "renderer3d.h"
#include "config.h"
#include "renderer3deventhandlers.h"

#include <QGradient>
#include <QWidget>
#include <QDebug>
#include <algorithm>
#include <array>
#include <QDebug>

static const char *vertexShaderSourceCore =
    "#version 120\n"
    "attribute vec4 vertex;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "void main() {\n"
    "   gl_Position = projMatrix * viewMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 120\n"
    "void main() {\n"
    "   gl_FragColor = vec4(0, 1.0, 1.0, 1.0);\n"
    "}\n";

Renderer3D::Renderer3D()
    : m_view(),
      m_cameraVelocity(),
      m_program(new QOpenGLShaderProgram),
      m_cubeIndices(QOpenGLBuffer::IndexBuffer)
{
    m_view.setToIdentity();
    m_view.translate(0, 0, -10);

    glClearColor(0, 0, 0, 1);

    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->bindAttributeLocation("vertex", 0);

    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_viewMatrixLoc = m_program->uniformLocation("viewMatrix");

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    std::array<QVector3D, 8> vertices = {QVector3D{-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,1.0f}, {-1.0f,1.0f,-1.0f}, {-1.0f,1.0f,1.0f}, {1.0f,-1.0f,-1.0f}, {1.0f,-1.0f,1.0f}, {1.0f,1.0f,-1.0f}, {1.0f,1.0f,1.0f}};
    std::array<GLushort, 16> vertexIndices = {2,0,6,4,5,0,1,2,3,6,7,5,3,1};

    // Setup our vertex buffer object.
    m_cubeVertices.create();
    m_cubeVertices.bind();
    m_cubeVertices.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

    m_cubeIndices.create();
    m_cubeIndices.bind();
    m_cubeIndices.allocate(vertexIndices.data(), vertexIndices.size() * sizeof(GLushort));

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

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

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);

    m_vao.bind();

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_viewMatrixLoc, m_view);


    // Draw cube geometry using indices from VBO 1
    f->glDrawElements(GL_TRIANGLE_STRIP, 16, GL_UNSIGNED_SHORT, 0);

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
