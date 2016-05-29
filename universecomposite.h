#ifndef UNIVERSECOMPOSITE_H
#define UNIVERSECOMPOSITE_H

#include "universecomponent.h"
#include "renderer.h"
#include <string>
#include <vector>
#include <memory>
#include <utility>


class UniverseComposite : public UniverseComponent {

public:
    UniverseComposite(
            UniverseComponentType type,
            const std::string& name,
            const std::string& parentName = "");
    virtual ~UniverseComposite() { }

    //build up the composition
    virtual void add(std::unique_ptr<UniverseComponent> component) { m_children.push_back(std::move(component)); }

    /*********************************************
     * Inherited methods from UniverseComponent
     * *******************************************/
    virtual void render(Renderer& painter) const;
    virtual void renderLabel(Renderer& painter) const;
    virtual void addAttractionTo(UniverseBody& other) const;
    virtual void resetForces();
    virtual void addAttractionFrom(const UniverseComponent& component);
    virtual void updatePosition(int timestep);

    std::pair<QVector3D, QVector3D> getBoundingBox() const;

    /*********************************************
     * Methods used to enable construction of the universe with relative positions
     * *******************************************/
    void setPosition(QVector3D position) { m_position = position; }
    void setVelocity(QVector3D velocity) { m_velocity = velocity; }
    //propagates the position and velocity of each object down to it's children
    //this should only be called ONCE
    void convertRelativeToAbsolute(QVector3D p, QVector3D v);

private:
    std::vector<std::unique_ptr<UniverseComponent>> m_children;

    //used only to enable construction of the universe with relative positions
    QVector3D m_velocity;
    QVector3D m_position;

};

#endif // UNIVERSECOMPOSITE_H
