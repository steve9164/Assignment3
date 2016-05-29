#include <QString>
#include <QtTest>
#include "../config.h"
#include "../universecomposite.h"

// Tests on UniverseComponent, Zodiac and Config systems

class Assignment_3 : public QObject
{
    Q_OBJECT

public:
    Assignment_3() { }

private Q_SLOTS:
    void testConfigValues();
    void testUniverseName();
    void testUniverseType();
    void testSolarSystemBBox();
    void testZodiacs();
    void testUniverseRetrieval();
    void testRelativeValues();
    void testAttractionX();
    void testAttractionXYZ();
};

void Assignment_3::testConfigValues()
{
    Config* config = Config::getInstance();

    config->read("test1.txt");
    qFuzzyCompare(config->getFramesPerSecond(), 60.0);
    QCOMPARE(config->getPhysicsStepSize(), 40000);
    QCOMPARE(config->getOvercalculatePhysicsAmount(), 10);
    qFuzzyCompare(config->getDistanceScale(), 1.0e10);
}

void Assignment_3::testUniverseName()
{
    Config* config = Config::getInstance();

    config->read("test1.txt");

    std::unique_ptr<UniverseComponent> u = config->retrieveUniverse();
    qDebug() << QString::fromStdString(u->getName());
    QCOMPARE(u->getName(), std::string("The Universe"));

}

void Assignment_3::testUniverseType()
{
    Config* config = Config::getInstance();

    config->read("test1.txt");

    std::unique_ptr<UniverseComponent> u = config->retrieveUniverse();
    qDebug() << QString::fromStdString(u->getName());
    QCOMPARE(u->getType(), UniverseComponentType::universe);

}

void Assignment_3::testSolarSystemBBox()
{
    Config* config = Config::getInstance();

    config->read("solarSystem.txt");

    std::unique_ptr<UniverseComponent> u = config->retrieveUniverse();
    auto bb = u->getBoundingBox();
    QVector3D expectedMin(-696e6, -696e6, -696e6);
    QVector3D expectedMax(4436.8e9, 696e6, 696e6);

    for (int i = 0; i < 3; i++)
    {
        qFuzzyCompare(bb.first[i], expectedMin[i]);
        qFuzzyCompare(bb.second[i], expectedMax[i]);
    }
}

void Assignment_3::testZodiacs()
{
    Config* config = Config::getInstance();

    config->read("zodiacs.txt");

    std::list<Zodiac> zodiacs = config->retrieveZodiacs();
    QCOMPARE(zodiacs.size(), (size_t)3);
}

void Assignment_3::testUniverseRetrieval()
{
    Config* config = Config::getInstance();

    config->read("solarSystem.txt");

    std::unique_ptr<UniverseComponent> u = config->retrieveUniverse();
    auto bb = u->getBoundingBox();
    QVector3D expectedMin(-696e6, -696e6, -696e6);
    QVector3D expectedMax(4436.8e9, 696e6, 696e6);

    for (int i = 0; i < 3; i++)
    {
        qFuzzyCompare(bb.first[i], expectedMin[i]);
        qFuzzyCompare(bb.second[i], expectedMax[i]);
    }

    // Universe already retrieved, config shouldn't have ownership of it any more
    u = config->retrieveUniverse();
    QVERIFY(u.release() == nullptr);
}

void Assignment_3::testRelativeValues()
{
    // Change the position of comp and body through relative to absolute conversion
    UniverseComposite comp(solarsystem, "1");
    comp.setPosition({5,6,7});
    UniverseBody* body = new UniverseBody(planet, "body", "1");
    body->setPosition({1,0,1});
    comp.add(std::unique_ptr<UniverseComponent>(body));
    comp.convertRelativeToAbsolute({-2,-3,-1}, {0,0,0});

    // Technically this is undefined behaviour since I don't own this pointer any more,
    // but it's still testing the public interface of UniverseComposite, not implementation details
    QVector3D diff = body->getPosition() - QVector3D({4,3,7});
    for (int i = 0; i < 3; i++)
    {
        // qFuzzyCompare can't compare at 0, so add 1 to each
        qFuzzyCompare(diff[i]+1, 1);
    }
}

void Assignment_3::testAttractionX()
{
   UniverseBody a(planet, "a");
   a.setPosition({0,0,0});
   a.addForce({1e4,0,0});
   a.updatePosition(40000);
   QVector3D diff = a.getPosition().normalized() - QVector3D({1,0,0});
   for (int i = 0; i < 3; i++)
   {
       // qFuzzyCompare can't compare at 0, so add 1 to each
       qFuzzyCompare(diff[i]+1, 1);
   }
}

void Assignment_3::testAttractionXYZ()
{
   UniverseBody a(planet, "a");
   a.setPosition({0,0,0});
   a.addForce({1e4,-1e4,1e4});
   a.updatePosition(40000);
   QVector3D diff = a.getPosition().normalized() - QVector3D({1,-1,1}).normalized();
   for (int i = 0; i < 3; i++)
   {
       // qFuzzyCompare can't compare at 0, so add 1 to each
       qFuzzyCompare(diff[i]+1, 1);
   }
}

QTEST_APPLESS_MAIN(Assignment_3)

#include "tst_assignment_3.moc"
