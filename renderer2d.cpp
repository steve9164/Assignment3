#include "renderer2d.h"
#include "config.h"
#include "renderer2deventhandlers.h"

#include <QGradient>
#include <QWidget>
#include <QDebug>

Renderer2D::Renderer2D()
    : m_cameraScale(1.0f),
      m_cameraPosition(),
      m_cameraVelocity()
{ }

std::shared_ptr<EventHandler> Renderer2D::buildEventChain()
{
    std::shared_ptr<EventHandler> handler(new KeyEventHandler(*this));
    handler->chain(std::shared_ptr<EventHandler>(new MouseWheelEventHandler(*this)));
    return handler;
}


void Renderer2D::startRender(QWidget* widget)
{
    m_cameraPosition += m_cameraVelocity;
    // Make a new QPainter to render on widget
    m_painter.reset(new QPainter(widget));

    //offset the painter so (0,0) is the center of the window
    m_painter->translate(QPointF(widget->width()/2, widget->height()/2) - m_cameraPosition);
    m_painter->scale(m_cameraScale, m_cameraScale);
}

void Renderer2D::drawBody(const UniverseBody& body)
{
    Config* config = Config::getInstance();

    //get scaled position and radius
    QVector3D pos = body.getPosition() / config->getDistanceScale();
    double radius = body.getRadius() / config->getRadiusScale();

    if(config->getUseLogRadius())
    {
        radius = std::log(body.getRadius() / config->getLogPointRadius());
    }

    if(radius < 1)
    {
        m_painter->setPen(body.getColor());
        m_painter->drawPoint(pos.x(), pos.y());
    }
    else
    {
        //no outline
        m_painter->setPen(Qt::NoPen);

        //gradient brush
        QRadialGradient gradient(pos.toPointF(), radius);
        gradient.setColorAt(0.25, body.getColor());
        gradient.setColorAt(1, Qt::transparent);

        m_painter->setBrush(gradient);

        m_painter->drawEllipse(pos.toPointF(), radius, radius);
    }
}

void Renderer2D::drawZodiac(const Zodiac& zodiac)
{
    double distanceScale = Config::getInstance()->getDistanceScale();
    m_painter->setPen(Qt::white);
    m_painter->setBrush(QColor(Qt::white));
    for(auto pair : zodiac.lines) {
        QVector3D p1 = pair.first.getPosition() / distanceScale;
        QVector3D p2 = pair.second.getPosition() / distanceScale;
        m_painter->drawLine(QLineF(p1.toPointF(), p2.toPointF()));
    }
}

void Renderer2D::drawLabel(const UniverseBody& body)
{
    Config* config = Config::getInstance();

    //get scaled position
    QVector3D pos = body.getPosition() / config->getDistanceScale();
    //draw the label
    m_painter->setPen(body.getColor());
    m_painter->drawText(QRectF(pos.x(), pos.y(), 150.0, 50.0), body.getName().c_str());
}


void Renderer2D::finishRender()
{
    // Delete the QPainter
    m_painter.reset();
}