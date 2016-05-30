#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "renderer.h"
#include "eventhandler.h"
#include <memory>
#include <QPainter>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Unfinished extension functionality


class Renderer3D : public Renderer
{
public:
    Renderer3D();

    ~Renderer3D();

    // Renderer interface
    std::shared_ptr<EventHandler> buildEventChain();
    void autoAdjustCamera(std::pair<QVector3D, QVector3D> boundingBox);


    void startRender(QWidget* widget);
    void drawBody(const UniverseBody &body);
    void drawZodiac(const Zodiac &zodiac);
    void drawLabel(const UniverseBody &body);
    void finishRender();

private:
    QWidget* m_widget;
    std::unique_ptr<QPainter> m_painter;
    QMatrix4x4 m_view;
    QMatrix4x4 m_proj;
    QVector3D m_cameraVelocity;

    std::unique_ptr<QOpenGLShaderProgram> m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_cubeVertices;
    QOpenGLBuffer m_cubeIndices;
    GLuint m_viewMatrixLoc;
    GLuint m_projMatrixLoc;

    // Event handlers (in renderer3deventhandlers.h)
    class KeyEventHandler;
    class MouseWheelEventHandler;

};

#endif // RENDERER3D_H