#pragma once
#include "PersonaBase.h"

struct PersonaDb
{
    static PersonaBase orpheus;
    static PersonaBase forneus;
    static PersonaBase hermes;
    static PersonaBase io;

    static void initialize();

  private:
    static Skill* orpheusSkills[2];
    static Skill* forneusSkills[2];
    static Skill* hermesSkills[1];
    static Skill* ioSkills[3];
};
