#ifndef UNIVERSEBODY_H
#define UNIVERSEBODY_H

#include <QVector3D>

#include "universecomponent.h"

class UniverseBody : public UniverseComponent
{
public:
    UniverseBody(
            UniverseComponentType type,
            const std::string& name,
            const std::string& parentName = "");

    virtual ~UniverseBody() {}

    /*********************************************
     * Inherited methods from UniverseComponent
     * *******************************************/
    //render the subtree
    virtual void render(Renderer& renderer) const;
    virtual void renderLabel(Renderer& renderer) const;

    //apply the attraction from this component, to the specified leaf
    virtual void addAttractionTo(UniverseBody& body) const;

    //update attractive forces to all components of this object, from the specified component
    virtual void addAttractionFrom(const UniverseComponent& component);

    //reset the accumulated forces to zero
    virtual void resetForces();

    //update the positions of all components of this object
    virtual void updatePosition(int timestep);

    //convert the initial (relative) position and velocity, to an absolute one
    //by translating the position and velocity with the values provided
    void convertRelativeToAbsolute(QVector3D p, QVector3D v) {
        m_position += p;
        m_velocity += v;
    }


    /*********************************************
     * Accessor methods
     * *******************************************/
    const QColor& getColor() const { return m_color; }
    QVector3D getPosition() const { return m_position; }
    double getMass() const { return m_mass; }
    double getRadius() const { return m_radius; }

    void addForce(QVector3D force) { m_force += force; }
    void setPosition(QVector3D position) { m_position += position; }
    void setVelocity(QVector3D velocity) { m_velocity = velocity; }
    void setRadius(const double& radius) { m_radius = radius; }
    void setMass(const double& mass) { m_mass = mass; }
    void setColor(QColor color) { m_color = color; }

private:
    QVector3D m_force;

    QVector3D m_velocity;
    QVector3D m_position;

    double m_radius;
    double m_mass;
    QColor m_color;
};

#endif // UNIVERSEBODY_H
