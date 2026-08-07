/**
 * @file GenericEvents.hpp
 * @brief Generic events used across multiple systems
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "core/structs.h"
#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief Event payload to set the text component.
 *
 * @details A lot of systems require the text componen to be
 * set before they print any text. Otherwise, it is possible for such
 * systems to throw errors.
 */
struct SetTextComponent : public etl::message<EventID::SetTextComponent>
{
    TextComponent* text = nullptr;
    SetTextComponent(TextComponent* iText) : text(iText)
    {
    }
};

/**
 * @brief Event payload to set the character position data.
 */
struct SetCharacterPosition : public etl::message<EventID::SetCharacterPosition>
{
    CharacterPosition charPos;
    SetCharacterPosition(CharacterPosition iCharPos) : charPos(iCharPos)
    {
    }
};
} // namespace Event
