#ifndef DIALOG_H
#define DIALOG_H

#include "config.h"
#include "universecomponent.h"
#include "zodiac.h"
#include "renderer.h"
#include <QDialog>
#include <QTimer>
#include <list>
#include <QOpenGLWidget>

class EventHandler;


namespace Ui {
class Dialog;
}

class Dialog : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    virtual ~Dialog();

private slots:
    //prepare the next frame, called by the timer
    void nextFrame();
    //toggle the simultation's pause state
    void togglePause();
    //toggle rendering of Zodiacs
    void toggleZodiacs();
    //toggle rendering of labels
    void toggleLabels();
    //auto-adjust view
    void autoAdjust();

    // adjust physics simulation rate
    void slower();
    void faster();

private:
    void initializeGL();
    //method called when the window is being redrawn
    void paintGL();
    //pause (or unpause) the simulation
    void pause(bool pause);
    //handle key presses
    bool event(QEvent *event);

private:
    Ui::Dialog* ui;
    QTimer* m_timer; //Timer object for triggering updates

    //buttons for UI
    QPushButton* m_buttonPause;
    QPushButton* m_buttonZodiacs;
    QPushButton* m_buttonLabels;
    QPushButton* m_buttonAutoAdjust;
    QPushButton* m_buttonSlower;
    QPushButton* m_buttonFaster;

    int m_width; //width of the window
    int m_height; //height of the window
    bool m_paused; //is the simulation paused?
    bool m_renderZodiacs; //should Zodiacs be rendered?
    bool m_renderLabels; //should labels be rendered?
    bool m_render3d; //should rendering be done in OpenGL 3D?
    long m_timestamp; //simulation time since simulation start
    std::unique_ptr<UniverseComponent> m_universe; //The universe
    std::list<Zodiac> m_zodiacs; //Vector of zodiac lines
    Config* m_config; //the singleton config instance
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<EventHandler> m_eventHandler;

    class KeyEventHandler;

};

#endif // DIALOG_H
