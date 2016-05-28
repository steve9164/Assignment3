#include "universecomposite.h"

UniverseComposite::UniverseComposite(
        UniverseComponentType type,
        const std::string& name,
        const std::string& parentName)
        : UniverseComponent(type, name, parentName)
        , m_xVelocity(0.0)
        , m_yVelocity(0.0)
        , m_xPosition(0.0)
        , m_yPosition(0.0)
{ }

//render the subtree
void UniverseComposite::render(QPainter& painter) const
{
    for(auto& child : m_children)
    {
        child->render(painter);
    }
}

void UniverseComposite::renderLabel(QPainter& painter) const
{
    for(auto& child : m_children)
    {
        child->renderLabel(painter);
    }
}


//apply the attraction from this component, to the specified leaf
void UniverseComposite::addAttractionTo(UniverseBody& other) const
{
    for(auto& child : m_children)
    {
        child->addAttractionTo(other);
    }
}

void UniverseComposite::resetForces()
{
    for(auto& child : m_children)
    {
        child->resetForces();
    }
}

//update attractive forces to all components of this object, from the specified component
void UniverseComposite::addAttractionFrom(const UniverseComponent& component)
{
    for(auto& child : m_children)
    {
        child->addAttractionFrom(component);
    }
}

//update the positions of all components of this object
void UniverseComposite::updatePosition(int timestep)
{
    for(auto& child : m_children)
    {
        child->updatePosition(timestep);
    }
}




void UniverseComposite::convertRelativeToAbsolute(double xp, double yp, double xv, double yv)
{
    xp += m_xPosition;
    yp += m_yPosition;
    xv += m_xVelocity;
    yv += m_yVelocity;
    for(auto& child : m_children)
    {
        child->convertRelativeToAbsolute(xp, yp, xv, yv);
    }
}

