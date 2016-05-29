#include "renderer2deventhandlers.h"

constexpr float CAMERA_SPEED = 5.0f;
constexpr float CAMERA_ZOOM_FACTOR = 2000.0f;

Renderer2D::KeyEventHandler::KeyEventHandler(Renderer2D& renderer)
    : m_renderer(renderer)
{ }

bool Renderer2D::KeyEventHandler::handleEvent(QEvent* event)
{
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (keyEvent && handleKeyEvent(keyEvent))
    {
        return true;
    }
    else
    {
        return m_next->handleEvent(event);
    }
}

bool Renderer2D::KeyEventHandler::handleKeyEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat())
    {
        return false;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        switch (event->key())
        {
        case Qt::Key_W:
            m_renderer.m_cameraVelocity += QPointF(0, -CAMERA_SPEED);
            return true;
        case Qt::Key_A:
            m_renderer.m_cameraVelocity += QPointF(-CAMERA_SPEED, 0);
            return true;
        case Qt::Key_S:
            m_renderer.m_cameraVelocity += QPointF(0, CAMERA_SPEED);
            return true;
        case Qt::Key_D:
            m_renderer.m_cameraVelocity += QPointF(CAMERA_SPEED, 0);
            return true;
        default:
            return false;
        }
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        switch (event->key())
        {
        case Qt::Key_W:
            m_renderer.m_cameraVelocity -= QPointF(0, -CAMERA_SPEED);
            return true;
        case Qt::Key_A:
            m_renderer.m_cameraVelocity -= QPointF(-CAMERA_SPEED, 0);
            return true;
        case Qt::Key_S:
            m_renderer.m_cameraVelocity -= QPointF(0, CAMERA_SPEED);
            return true;
        case Qt::Key_D:
            m_renderer.m_cameraVelocity -= QPointF(CAMERA_SPEED, 0);
            return true;
        default:
            return false;
        }
    }
    else
    {
        return false;
    }
}

Renderer2D::MouseWheelEventHandler::MouseWheelEventHandler(Renderer2D& renderer)
    : m_renderer(renderer)
{ }

bool Renderer2D::MouseWheelEventHandler::handleEvent(QEvent* event)
{
    QWheelEvent* mouseWheelEvent = dynamic_cast<QWheelEvent*>(event);
    if (mouseWheelEvent && handleMouseWheelEvent(mouseWheelEvent))
    {
        return true;
    }
    else
    {
        return m_next->handleEvent(event);
    }
}

bool Renderer2D::MouseWheelEventHandler::handleMouseWheelEvent(QWheelEvent* event)
{
    if (!event->angleDelta().isNull())
    {
        m_renderer.m_cameraScale *= 1 + (event->angleDelta().y() / CAMERA_ZOOM_FACTOR);
        return true;
    }
    else
    {
        return false;
    }
}
