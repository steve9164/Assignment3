#include "dialog.h"
#include "ui_dialog.h"

#include "universecomposite.h"
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , m_width(800)
    , m_height(800)
    , m_paused(false)
    , m_renderZodiacs(true)
    , m_renderLabels(true)
    , m_timestamp(0)
    , m_config(Config::getInstance())
{
    m_config->read("config.txt");
    m_universe = m_config->parseUniverseBlocks();
    m_zodiacs = m_config->parseZodiacBlocks();
    m_universe->convertRelativeToAbsolute(0,0,0,0);

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
    m_buttonPause->setGeometry(QRect(QPoint(0, 0), QSize(100, 50)));
    m_buttonZodiacs->setGeometry(QRect(QPoint(100, 0), QSize(100, 50)));
    m_buttonLabels->setGeometry(QRect(QPoint(200, 0), QSize(100, 50)));
    connect(m_buttonPause, SIGNAL(released()), this, SLOT(togglePause()));
    connect(m_buttonZodiacs, SIGNAL(released()), this, SLOT(toggleZodiacs()));
    connect(m_buttonLabels, SIGNAL(released()), this, SLOT(toggleLabels()));

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
    delete m_universe;
    delete m_zodiacs;
}

void Dialog::toggleZodiacs()
{
    m_renderZodiacs = !m_renderZodiacs;
}

void Dialog::toggleLabels()
{
    m_renderLabels = !m_renderLabels;
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

void Dialog::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Space:
        pause(!m_paused);
        return;
    default:
        return;
    }
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

void Dialog::paintEvent(QPaintEvent *event)
{
    //suppress 'unused variable' warning
    Q_UNUSED(event);

    //redraw the universe
    QPainter painter(this);

    //offset the painter so (0,0) is the center of the window
    painter.translate(m_width/2, m_height/2);

    if(m_renderZodiacs)
    {
        for(auto zodiac : *m_zodiacs)
        {
            zodiac.render(painter);
        }
    }

    m_universe->render(painter);

    if(m_renderLabels)
    {
        m_universe->renderLabel(painter);
    }
}




