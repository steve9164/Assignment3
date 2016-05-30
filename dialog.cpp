#include "dialog.h"
#include "ui_dialog.h"

#include "universecomposite.h"
#include "renderer2d.h"
#include "renderer3d.h"
#include "eventhandler.h"
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QSurfaceFormat>

class Dialog::KeyEventHandler : public EventHandler
{
public:
    KeyEventHandler(Dialog& dialog);
    bool handleEvent(QEvent* event);

private:
    bool handleKeyEvent(QKeyEvent* event);

    Dialog& m_dialog;
};

Dialog::Dialog(QWidget *parent)
    : QOpenGLWidget(parent)
    , ui(new Ui::Dialog)
    , m_width(800)
    , m_height(800)
    , m_paused(false)
    , m_renderZodiacs(true)
    , m_renderLabels(true)
    , m_render3d(false)
    , m_timestamp(0)
    , m_config(Config::getInstance())
    , m_eventHandler(new EventHandler::NoopEventHandler())
{
    m_config->read("config.txt");
    m_universe = m_config->retrieveUniverse();
    m_zodiacs = m_config->retrieveZodiacs();
    m_universe->convertRelativeToAbsolute(QVector3D(), QVector3D());

    //set backdrop to black
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);

    //make the window appear
    ui->setupUi(this);
    this->resize(m_width, m_height);

    //create the buttons
    m_buttonPause = new QPushButton("Pause", this);
    m_buttonZodiacs = new QPushButton("Zodiacs", this);
    m_buttonLabels = new QPushButton("Labels", this);
    m_buttonAutoAdjust = new QPushButton("Auto-adjust", this);
    m_buttonSlower = new QPushButton("Slower", this);
    m_buttonFaster = new QPushButton("Faster", this);
    m_buttonPause->setGeometry(QRect(QPoint(0, 0), QSize(100, 50)));
    m_buttonZodiacs->setGeometry(QRect(QPoint(100, 0), QSize(100, 50)));
    m_buttonLabels->setGeometry(QRect(QPoint(200, 0), QSize(100, 50)));
    m_buttonAutoAdjust->setGeometry(QRect(QPoint(300, 0), QSize(100, 50)));
    m_buttonSlower->setGeometry(QRect(QPoint(400, 0), QSize(100, 50)));
    m_buttonFaster->setGeometry(QRect(QPoint(500, 0), QSize(100, 50)));
    connect(m_buttonPause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(m_buttonZodiacs, SIGNAL(released()), this, SLOT(toggleZodiacs()));
    connect(m_buttonLabels, SIGNAL(released()), this, SLOT(toggleLabels()));
    connect(m_buttonAutoAdjust, SIGNAL(released()), this, SLOT(autoAdjust()));
    connect(m_buttonSlower, SIGNAL(released()), this, SLOT(slower()));
    connect(m_buttonFaster, SIGNAL(released()), this, SLOT(faster()));

    //setup timer
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    pause(false);
}

Dialog::~Dialog()
{
    delete ui;
    delete m_timer;
    delete m_buttonPause;
    delete m_buttonZodiacs;
    delete m_buttonLabels;
}

void Dialog::toggleZodiacs()
{
    m_renderZodiacs = !m_renderZodiacs;
}

void Dialog::toggleLabels()
{
    m_renderLabels = !m_renderLabels;
}

void Dialog::autoAdjust()
{
    m_renderer->autoAdjustCamera(m_universe->getBoundingBox());
}

void Dialog::slower()
{
    // Slow physics rate by 1.5 times and round to integer
    m_config->setPhysicsStepSize(m_config->getPhysicsStepSize()*0.66 + 0.5);
}

void Dialog::faster()
{
    // Speed up physics rate by 1.5 times and round to integer
    m_config->setPhysicsStepSize(m_config->getPhysicsStepSize()*1.5 + 0.5);
}

void abcinit() {
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(200, 0, 0, 1);
}

void Dialog::initializeGL()
{
    m_renderer.reset(m_render3d ? static_cast<Renderer*>(new Renderer3D()) : static_cast<Renderer*>(new Renderer2D()));

    // Build Dialog event chain
    m_eventHandler.reset(new KeyEventHandler(*this));
    // Add renderer event chain
    m_eventHandler->chain(m_renderer->buildEventChain());
}

void drawSomething()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}

void Dialog::paintGL()
{
    m_renderer->startRender(this);

    if(m_renderZodiacs)
    {
        for(auto& zodiac : m_zodiacs)
        {
            zodiac.render(*m_renderer);
        }
    }

    m_universe->render(*m_renderer);

    if(m_renderLabels)
    {
        m_universe->renderLabel(*m_renderer);
    }

    m_renderer->finishRender();
}

void Dialog::togglePause()
{
    pause(!m_paused);
}

void Dialog::pause(bool pause)
{
    if(pause)
    {
        m_timer->stop();
        m_paused = true;
    }
    else
    {
        m_timer->start(1000 / m_config->getFramesPerSecond());
        m_paused = false;
    }
}

bool Dialog::event(QEvent* event)
{
    return m_eventHandler->handleEvent(event) || QOpenGLWidget::event(event);
}

void Dialog::nextFrame()
{
    //reset the forces stored in every object to 0
    m_universe->resetForces();

    //update the forces acting on every object in the universe,
    //from every object in the universe
    m_universe->addAttractionFrom(*m_universe);

    //update the velocity and position of every object in the universe
    int step = m_config->getPhysicsStepSize() / m_config->getOvercalculatePhysicsAmount();
    for(int i = 0; i < m_config->getPhysicsStepSize(); i += step)
    {
        //update physics
        m_universe->updatePosition(step);
        //some time has passed
        m_timestamp += step;
    }

    //update the window (this will trigger paintEvent)
    update();
}


Dialog::KeyEventHandler::KeyEventHandler(Dialog &dialog)
    : m_dialog(dialog)
{ }

bool Dialog::KeyEventHandler::handleEvent(QEvent *event)
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

bool Dialog::KeyEventHandler::handleKeyEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return false;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        switch (event->key())
        {
        case Qt::Key_Space:
            m_dialog.pause(!m_dialog.m_paused);
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
