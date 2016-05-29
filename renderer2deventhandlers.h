#ifndef RENDERER2DEVENTHANDLERS_H
#define RENDERER2DEVENTHANDLERS_H

#include "renderer2d.h"


class Renderer2D::KeyEventHandler : public EventHandler
{
public:
    KeyEventHandler(Renderer2D& renderer);
    bool handleEvent(QEvent* event);

private:
    bool handleKeyEvent(QKeyEvent* event);

    Renderer2D& m_renderer;
};


class Renderer2D::MouseWheelEventHandler : public EventHandler
{
public:
    MouseWheelEventHandler(Renderer2D& renderer);
    bool handleEvent(QEvent* event);

private:
    bool handleMouseWheelEvent(QWheelEvent* event);

    Renderer2D& m_renderer;
};


#endif // RENDERER2DEVENTHANDLERS_H
