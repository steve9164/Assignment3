#include "renderer3deventhandlers.h"

constexpr float CAMERA_SPEED = 5.0f;
constexpr float CAMERA_ZOOM_FACTOR = 500.0f;

Renderer3D::KeyEventHandler::KeyEventHandler(Renderer3D& renderer)
    : m_renderer(renderer)
{ }

bool Renderer3D::KeyEventHandler::handleEvent(QEvent* event)
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

bool Renderer3D::KeyEventHandler::handleKeyEvent(QKeyEvent* event)
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
            m_renderer.m_cameraVelocity += QVector3D(0, -CAMERA_SPEED, 0);
            return true;
        case Qt::Key_A:
            m_renderer.m_cameraVelocity += QVector3D(-CAMERA_SPEED, 0, 0);
            return true;
        case Qt::Key_S:
            m_renderer.m_cameraVelocity += QVector3D(0, CAMERA_SPEED, 0);
            return true;
        case Qt::Key_D:
            m_renderer.m_cameraVelocity += QVector3D(CAMERA_SPEED, 0, 0);
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
            m_renderer.m_cameraVelocity -= QVector3D(0, -CAMERA_SPEED, 0);
            return true;
        case Qt::Key_A:
            m_renderer.m_cameraVelocity -= QVector3D(-CAMERA_SPEED, 0, 0);
            return true;
        case Qt::Key_S:
            m_renderer.m_cameraVelocity -= QVector3D(0, CAMERA_SPEED, 0);
            return true;
        case Qt::Key_D:
            m_renderer.m_cameraVelocity -= QVector3D(CAMERA_SPEED, 0, 0);
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

Renderer3D::MouseWheelEventHandler::MouseWheelEventHandler(Renderer3D& renderer)
    : m_renderer(renderer)
{ }

bool Renderer3D::MouseWheelEventHandler::handleEvent(QEvent* event)
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

bool Renderer3D::MouseWheelEventHandler::handleMouseWheelEvent(QWheelEvent* event)
{
    if (!event->angleDelta().isNull())
    {
        m_renderer.m_view.translate(0, 0, (event->angleDelta().y() / CAMERA_ZOOM_FACTOR));
        return true;
    }
    else
    {
        return false;
    }
}
