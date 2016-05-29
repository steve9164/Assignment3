#include "universecomposite.h"

#include <algorithm>
#include <iterator>
#include <QDebug>

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

// Compare the Nth dimension of the min in the pair
struct CompMin
{
    int n;
    CompMin(int n): n(n) { }
    bool operator() (const std::pair<QVector3D, QVector3D>& a, const std::pair<QVector3D, QVector3D>& b)
    {
        // pairs are {min, max}
        return a.first[n] < b.first[n];
    }
};



// Compare the Nth dimension of the max in the pair
struct CompMax
{
    int n;
    CompMax(int n): n(n) { }
    bool operator() (const std::pair<QVector3D, QVector3D>& a, const std::pair<QVector3D, QVector3D>& b)
    {
        // pairs are {min, max}
        return a.second[n] < b.second[n];
    }
};

std::pair<QVector3D, QVector3D> UniverseComposite::getBoundingBox() const
{
    if (m_children.size() == 0)
        return {QVector3D(), QVector3D()};
    // Get bounding boxes of children
    std::vector<std::pair<QVector3D, QVector3D>> boundingBoxes;
    std::transform(m_children.begin(), m_children.end(), std::back_inserter(boundingBoxes),
                   [](const std::unique_ptr<UniverseComponent>& a) { return a->getBoundingBox(); });
    qDebug() << boundingBoxes.size() << m_children.size();
    // Calculate overall bounding box
    QVector3D min, max;
    for (int i = 0; i < 3; i++)
    {
        min[i] = std::min_element(boundingBoxes.begin(), boundingBoxes.end(), CompMin(i))->first[i];
        max[i] = std::max_element(boundingBoxes.begin(), boundingBoxes.end(), CompMax(i))->second[i];
    }
    return {min, max};
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

