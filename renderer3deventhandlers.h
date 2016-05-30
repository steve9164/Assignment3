#ifndef RENDERER3DEVENTHANDLERS_H
#define RENDERER3DEVENTHANDLERS_H

#include "renderer3d.h"


class Renderer3D::KeyEventHandler : public EventHandler
{
public:
    KeyEventHandler(Renderer3D& renderer);
    bool handleEvent(QEvent* event);

private:
    bool handleKeyEvent(QKeyEvent* event);

    Renderer3D& m_renderer;
};


class Renderer3D::MouseWheelEventHandler : public EventHandler
{
public:
    MouseWheelEventHandler(Renderer3D& renderer);
    bool handleEvent(QEvent* event);

private:
    bool handleMouseWheelEvent(QWheelEvent* event);

    Renderer3D& m_renderer;
};


#endif // RENDERER3DEVENTHANDLERS_H
