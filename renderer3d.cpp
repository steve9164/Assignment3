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
    "#version 330\n"
    "in vec4 vertex;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "void main() {\n"
    "   gl_Position = projMatrix * viewMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 330\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0, 1.0, 1.0);\n"
    "}\n";

Renderer3D::Renderer3D()
    : m_view(),
      m_cameraVelocity(),
      m_program(new QOpenGLShaderProgram)
{
    m_view.setToIdentity();
//    m_view.translate(0, 0, -1);

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
    std::array<GLushort, 15> vertexIndices = {7,6,3,2,0,3,1,7,5,6,4,2,0,5,1};

    // Setup our vertex buffer object.
    m_cubeVertices.create();
    m_cubeVertices.bind();
    m_cubeVertices.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

    m_cubeIndices.create();
    m_cubeIndices.bind();
    m_cubeIndices.allocate(vertexIndices.data(), vertexIndices.size() * sizeof(GLushort));



    m_program->release();
}

Renderer3D::~Renderer3D()
{

}

std::shared_ptr<EventHandler> Renderer3D::buildEventChain()
{
    std::shared_ptr<EventHandler> handler(new KeyEventHandler(*this));
    handler->chain(std::shared_ptr<EventHandler>(new MouseWheelEventHandler(*this)));
    return handler;
}

void Renderer3D::autoAdjustCamera(std::pair<QVector3D, QVector3D> boundingBox)
{
//    Config* config = Config::getInstance();

//    QVector2D min = boundingBox.first.toVector2D();
//    QVector2D max = boundingBox.second.toVector2D();
//    QVector2D diag = (max-min) / config->getDistanceScale();
//    m_cameraPosition = (min/config->getDistanceScale() + diag/2).toPointF();
//    m_cameraScale = std::min(0.8*m_widget->width()/diag.x(), 0.8*m_widget->height()/diag.y());
}

void Renderer3D::startRender(QWidget* widget)
{
    m_widget = widget;
    //m_view.translate(-m_cameraVelocity);

    m_view.setToIdentity();

    m_view.translate(0,0,-1);


    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(widget->width()) / widget->height(), 0.01f, 100.0f);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_viewMatrixLoc, m_view);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 15, GL_UNSIGNED_SHORT, 0);

    m_program->release();

//    // Make a new QPainter to render on widget
//    m_painter.reset(new QPainter(widget));

//    //offset the painter so (0,0) is the center of the window
//    m_painter->translate(QPointF(widget->width()/2, widget->height()/2));
//    m_painter->scale(m_cameraScale, m_cameraScale);
//    m_painter->translate(-m_cameraPosition);

}

void Renderer3D::drawBody(const UniverseBody& body)
{
//    Config* config = Config::getInstance();

//    //get scaled position and radius
//    QVector3D pos = body.getPosition() / config->getDistanceScale();
//    double radius = body.getRadius() / config->getRadiusScale();

//    if(config->getUseLogRadius())
//    {
//        radius = std::log(body.getRadius() / config->getLogPointRadius());
//    }

//    if(radius < 1)
//    {
//        m_painter->setPen(body.getColor());
//        m_painter->drawPoint(pos.x(), pos.y());
//    }
//    else
//    {
//        //no outline
//        m_painter->setPen(Qt::NoPen);

//        //gradient brush
//        QRadialGradient gradient(pos.toPointF(), radius);
//        gradient.setColorAt(0.25, body.getColor());
//        gradient.setColorAt(1, Qt::transparent);

//        m_painter->setBrush(gradient);

//        m_painter->drawEllipse(pos.toPointF(), radius, radius);
//    }
}

void Renderer3D::drawZodiac(const Zodiac& zodiac)
{
//    double distanceScale = Config::getInstance()->getDistanceScale();
//    m_painter->setPen(Qt::white);
//    m_painter->setBrush(QColor(Qt::white));
//    for(auto pair : zodiac.lines) {
//        QVector3D p1 = pair.first.getPosition() / distanceScale;
//        QVector3D p2 = pair.second.getPosition() / distanceScale;
//        m_painter->drawLine(QLineF(p1.toPointF(), p2.toPointF()));
//    }
}

void Renderer3D::drawLabel(const UniverseBody& body)
{
//    Config* config = Config::getInstance();

//    //get scaled position
//    QVector3D pos = body.getPosition() / config->getDistanceScale();
//    //draw the label
//    m_painter->setPen(body.getColor());
//    m_painter->drawText(QRectF(pos.x(), pos.y(), 150.0, 50.0), body.getName().c_str());
}


void Renderer3D::finishRender()
{
//    // Delete the QPainter
//    m_painter.reset();
}
