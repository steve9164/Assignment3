#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "renderer.h"
#include "eventhandler.h"
#include <memory>
#include <QPainter>
#include <QKeyEvent>


class Renderer2D : public Renderer
{
public:
    Renderer2D();

    // Renderer interface
    std::shared_ptr<EventHandler> buildEventChain();


    void startRender(QWidget* widget);
    void drawBody(const UniverseBody &body);
    void drawZodiac(const Zodiac &zodiac);
    void drawLabel(const UniverseBody &body);
    void finishRender();

private:
    QWidget* m_widget;
    std::unique_ptr<QPainter> m_painter;
    QPointF m_cameraPosition;
    QPointF m_cameraVelocity;
    float m_cameraScale;

    // Event handlers (in renderer2deventhandlers.h)
    class KeyEventHandler;
    class MouseWheelEventHandler;

};

#endif // RENDERER2D_H
