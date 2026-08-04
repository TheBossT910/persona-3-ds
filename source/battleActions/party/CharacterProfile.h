#pragma once
#include "../ParticipantType.h"
#include "../ProfileBase.h"
#include "../personas/PersonaBase.h"
#include "../shoes/Shoe.h"
#include "../skills/Skill.h"
#include "../weapons/Weapon.h"
#include <etl/vector.h>
#include <nds.h>

/**
 * @brief Holds character data which a battleParticipant is created from
 *
 * @details
 * This datatype holds the current information of a character and
 * should in the future be synced after each battle so we can re-create the
 * characters correctly in a new batle
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */
struct CharacterProfile : ProfileBase
{
    ParticipantType participantType;

    ArmourType armourType;

    WeaponType weaponType;
    Weapon weapon;

    etl::vector<PersonaBase*, 13> personas;
    PersonaBase* curPersona;
};
