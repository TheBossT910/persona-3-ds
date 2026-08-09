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
 * @brief Event payload to set the subscreen text video buffer.
 *
 * @details A lot of systems require the text video buffer to be
 * set before they print any text. Otherwise, it is possible for such
 * systems to throw errors.
 */
struct SetTextVideoBufferSub : public etl::message<EventID::SetTextVideoBufferSub>
{
    /// Pointer to the text video buffer
    uint16_t* textVideoBufferSub = nullptr;
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
