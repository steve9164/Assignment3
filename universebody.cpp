#include "universebody.h"
#include "config.h"
#include <QDebug>
#include <QGradient>

UniverseBody::UniverseBody(
        UniverseComponentType type,
        const std::string& name,
        const std::string& parentName)
        : UniverseComponent(type, name, parentName)
        , m_xForce(0)
        , m_yForce(0)
        , m_xVelocity(0.0)
        , m_yVelocity(0.0)
        , m_xPosition(0.0)
        , m_yPosition(0.0)
        , m_radius(1.0)
        , m_mass(1.0)
        , m_color(Qt::white)
{ }

void UniverseBody::render(QPainter& painter) const
{
    Config* config = Config::getInstance();

    //get scaled position and radius
    double x = m_xPosition / config->getDistanceScale();
    double y = m_yPosition / config->getDistanceScale();
    double radius = m_radius / config->getRadiusScale();

    if(config->getUseLogRadius())
    {
        radius = std::log(m_radius / config->getLogPointRadius());
    }

    if(radius < 1)
    {
        painter.setPen(m_color);
        painter.drawPoint(x, y);
    }
    else
    {

        //no outline
        painter.setPen(Qt::NoPen);

        //gradient brush
        QRadialGradient gradient(x, y, radius);
        gradient.setColorAt(0.25, m_color);
        gradient.setColorAt(1, Qt::transparent);

        painter.setBrush(gradient);

        painter.drawEllipse(QPointF(x, y), radius, radius);
    }
}

void UniverseBody::renderLabel(QPainter& painter) const
{
    Config* config = Config::getInstance();

    //get scaled position
    double x = m_xPosition / config->getDistanceScale();
    double y = m_yPosition / config->getDistanceScale();
    //draw the label
    painter.setPen(m_color);
    painter.drawText(QRectF(x, y, 150.0, 50.0), getName().c_str());
}


void UniverseBody::addAttractionTo(UniverseBody &other) const
{
    if(this == &other)
    {
        return;
    }
    double other_mass = other.getMass();
    double dx = m_xPosition - other.getPositionX();
    double dy = m_yPosition - other.getPositionY();
    double distance_sq = dx*dx + dy*dy;
    if(distance_sq == 0.0)
    {
        return;
    }

    double force = GRAVITATIONAL_CONSTANT * m_mass * other_mass / distance_sq;

    double distance = sqrt(distance_sq);

    //normalise the vector {dx, dy} by dividing it by the distance, to get the direction
    other.addForce(force * (dx / distance), force * (dy / distance));
}

void UniverseBody::addAttractionFrom(const UniverseComponent &component)
{
    component.addAttractionTo(*this);
}

void UniverseBody::resetForces()
{
    m_xForce = 0.0;
    m_yForce = 0.0;
}

void UniverseBody::updatePosition(int timestep)
{
     // calculate acceleration
     double xAccel = m_xForce / m_mass;
     double yAccel = m_yForce / m_mass;

     // remember the old velocity
     double oldXVelocity = m_xVelocity;
     double oldYVelocity = m_yVelocity;

     // calculate the new velocity (integrate acceleration)
     m_xVelocity += xAccel * timestep;
     m_yVelocity += yAccel * timestep;

     // calculate the new position (integrate velocity)
     // slightly improved by using the average velocity during this timestep
     m_xPosition += (m_xVelocity + oldXVelocity)/2 * timestep;
     m_yPosition += (m_yVelocity + oldYVelocity)/2* timestep;

}

