#ifndef CONFIG_H
#define CONFIG_H

#include "universecomponent.h"
#include "zodiac.h"
#include <list>
#include <string>
#include <unordered_map>


class Config
{
public:
    virtual ~Config();

    //Singleton accessor method
    static Config* getInstance();

    //read the given file
    void read(const std::string& path);

    //give ownership and control of the universe defined in the config to the caller
    std::unique_ptr<UniverseComponent> retrieveUniverse() { return move(m_universe); }

    //move the list of zodiacs to the caller and return the list to a known state
    std::list<Zodiac> retrieveZodiacs() { return move(m_zodiacs); }

    //how many times should the screen update each second
    double getFramesPerSecond() const { return m_framesPerSecond; }

    //how many seconds should be simulated each time the physics is updated
    int getPhysicsStepSize() const { return m_physicsStepSize; }

    //split each physics step into this many sub-steps
    int getOvercalculatePhysicsAmount() const { return m_overcalculatePhysics; }

    //meters per pixel, between bodies
    double getDistanceScale() const { return m_distanceScale; }

    //meters per pixel, for body radius
    double getRadiusScale() const { return m_radiusScale; }

    //use a log2 scale for object radius, instead of getRadiusScale?
    bool getUseLogRadius() const { return m_useLogRadius; }

    //if using log2 scale, what radius in m is ~1 pixel?
    double getLogPointRadius() const { return m_logPointRadius; }

private:
    //private constructor because this is a Singleton
    Config();
    static bool s_initialised;
    static Config* s_instance;

    //read a settings block into the static (class) variables
    void parseSettings();

    //read the all the lines from a file, into a list
    void readLines(const std::string& path, std::list<std::string>&) const;

    //remove a //comment from a line
    std::string stripComment(const std::string& line) const;

    //is the given line a [heading]?
    bool isBlockHeading(const std::string& line) const;

    //is the given line a 'key = value' pair?
    bool isKeyValuePair(const std::string& key, const std::string& op, const std::string& value) const;

    //helper method to join two components (also checks it's an allowed connection)
    void joinComponents(UniverseComponent &parent, std::unique_ptr<UniverseComponent> child);

    //helper method to connect the two named objects in a zodiac
    void addToZodiac(const std::string& labelA, const std::string& labelB, Zodiac& zodiac, const std::unordered_map<std::string, UniverseComponent&>& references);

    void parseUniverseBlocks();
    void createZodiacs(const std::unordered_map<std::string, UniverseComponent&>& universeComponents);

    //member variables to store the parsed text
    std::unordered_map<std::string, std::string> m_settings;
    std::list<std::unordered_map<std::string, std::string> > m_universeBlocks;
    std::list<std::list<std::pair<std::string, std::string> > > m_zodiacBlocks;

    // built while parsing, and returned by move when "retrieved" to be owned by Dialog
    std::unique_ptr<UniverseComponent> m_universe;
    std::list<Zodiac> m_zodiacs;



    double m_framesPerSecond;
    int m_physicsStepSize;
    int m_overcalculatePhysics;
    double m_distanceScale;
    double m_radiusScale;
    bool m_useLogRadius;
    double m_logPointRadius;
};

#endif // CONFIG_H
