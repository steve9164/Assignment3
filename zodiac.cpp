#include "zodiac.h"
#include "config.h"
#include "renderer.h"
#include <utility>

Zodiac::Zodiac()
{
}

Zodiac::~Zodiac()
{
}

void Zodiac::render(Renderer& renderer) const
{
    renderer.drawZodiac(*this);
}

void Zodiac::add(UniverseBody& from, UniverseBody& to)
{
    lines.emplace_back(from, to);
}
