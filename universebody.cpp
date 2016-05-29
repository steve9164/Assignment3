#include "universebody.h"
#include "config.h"
#include "renderer.h"
#include <QDebug>


UniverseBody::UniverseBody(
        UniverseComponentType type,
        const std::string& name,
        const std::string& parentName)
        : UniverseComponent(type, name, parentName)
        , m_force()
        , m_velocity()
        , m_position()
        , m_radius(1.0)
        , m_mass(1.0)
        , m_color(Qt::white)
{ }

void UniverseBody::render(Renderer& renderer) const
{
    renderer.drawBody(*this);
}

void UniverseBody::renderLabel(Renderer& renderer) const
{
    renderer.drawLabel(*this);
}


void UniverseBody::addAttractionTo(UniverseBody &other) const
{
    if(this == &other)
    {
        return;
    }
    double other_mass = other.getMass();
    QVector3D d = m_position - other.getPosition();
    double distance_sq = d.lengthSquared();
    if(distance_sq == 0.0)
    {
        return;
    }

    double force = GRAVITATIONAL_CONSTANT * m_mass * other_mass / distance_sq;

    //normalise the vector by dividing it by the distance, to get the direction
    other.addForce(force * d.normalized());
}

void UniverseBody::addAttractionFrom(const UniverseComponent &component)
{
    component.addAttractionTo(*this);
}

void UniverseBody::resetForces()
{
    m_force = {0,0,0};
}

void UniverseBody::updatePosition(int timestep)
{
     // calculate acceleration
     QVector3D accel = m_force / m_mass;

     // remember the old velocity
     QVector3D oldVelocity = m_velocity;

     // calculate the new velocity (integrate acceleration)
     m_velocity += accel * timestep;

     // calculate the new position (integrate velocity)
     // slightly improved by using the average velocity during this timestep
     m_position += (m_velocity + oldVelocity)/2 * timestep;

}
