#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "renderer.h"
#include <memory>
#include <QPainter>


class Renderer2D : public Renderer
{
public:
    Renderer2D();

    // Renderer interface
    void startRender(QWidget* widget);
    void drawBody(const UniverseBody &body);
    void drawZodiac(const Zodiac &zodiac);
    void drawLabel(const UniverseBody &body);
    void finishRender();

private:
    QWidget* m_widget;
    std::unique_ptr<QPainter> m_painter;
};

#endif // RENDERER2D_H
