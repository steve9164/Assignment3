#ifndef ZODIAC_H
#define ZODIAC_H

#include "universebody.h"
#include <QPainter>
#include <list>
#include <utility>

class Renderer;

class Zodiac
{
public:
    Zodiac();
    virtual ~Zodiac();

    virtual void render(Renderer& renderer) const;

    //add a line to the zodiac, between the given bodies
    virtual void add(UniverseBody& from, UniverseBody& to);

    friend class Renderer2D;

private:
    //list of stored lines
    std::list<std::pair<UniverseBody&, UniverseBody&>> lines;

};

#endif // ZODIAC_H
