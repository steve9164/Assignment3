#include "universecomposite.h"

UniverseComposite::UniverseComposite(
        UniverseComponentType type,
        const std::string& name,
        const std::string& parentName)
        : UniverseComponent(type, name, parentName)
        , m_velocity()
        , m_position()
{ }

//render the subtree
void UniverseComposite::render(Renderer& renderer) const
{
    for(auto& child : m_children)
    {
        child->render(renderer);
    }
}

void UniverseComposite::renderLabel(Renderer& renderer) const
{
    for(auto& child : m_children)
    {
        child->renderLabel(renderer);
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




void UniverseComposite::convertRelativeToAbsolute(QVector3D p, QVector3D v)
{
    p += m_position;
    v += m_velocity;
    for(auto& child : m_children)
    {
        child->convertRelativeToAbsolute(p, v);
    }
}

