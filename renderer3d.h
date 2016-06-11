#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "renderer.h"
#include "eventhandler.h"
#include <memory>
#include <vector>


#include <QPainter>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include <QOpenGLFunctions_3_3_Core>

// Unfinished extension functionality

class Renderer3D : public Renderer, protected QOpenGLFunctions_3_3_Core
{
public:
    Renderer3D(QOpenGLWidget* widget);

    ~Renderer3D();

    // Renderer interface
    //void initialise();
    std::shared_ptr<EventHandler> buildEventChain();
    void autoAdjustCamera(std::pair<QVector3D, QVector3D> boundingBox);


    void startRender(QWidget* widget);
    void drawBody(const UniverseBody &body);
    void drawZodiac(const Zodiac &zodiac);
    void drawLabel(const UniverseBody &body);
    void finishRender();

private:
    QOpenGLWidget* m_widget;
    std::unique_ptr<QPainter> m_painter;
    QMatrix4x4 m_view;
    QMatrix4x4 m_proj;
    QVector3D m_cameraVelocity;

    // Temporaries for collecting
    struct BodyRenderDetails;
    std::vector<BodyRenderDetails> m_bodyRenderDetailsAccumulator;

    /*std::unique_ptr<QOpenGLShaderProgram>*/ QOpenGLShaderProgram*  m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_cubeVertices;
    QOpenGLBuffer m_cubeIndices;
    QOpenGLBuffer m_cubeInstanceData;
    GLuint m_viewMatrixLoc;
    GLuint m_projMatrixLoc;

    // Event handlers (in renderer3deventhandlers.h)
    class KeyEventHandler;
    class MouseWheelEventHandler;  
};

#endif // RENDERER3D_H
