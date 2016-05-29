#ifndef RENDERER_H
#define RENDERER_H

#include "zodiac.h"
#include "universebody.h"

// Abstract renderer class

class Zodiac;

class Renderer
{
public:
    Renderer() { }
    virtual ~Renderer() { }

    // called before each scene render
    virtual void startRender(QWidget*) { }

    // 3 methods called once per render per object
    virtual void drawBody(const UniverseBody&) = 0;
    virtual void drawZodiac(const Zodiac&) { }
    virtual void drawLabel(const UniverseBody&) { }

    // called after each scene render
    virtual void finishRender() { }

    // called when the widget is resized
    virtual void resizeScene(int, int) { }
};

#endif // RENDERER_H
