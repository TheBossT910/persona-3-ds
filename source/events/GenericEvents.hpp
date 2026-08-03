#pragma once
#include "core/enums.h"
#include <aegis/aegis.hpp>

namespace Event
{
struct SetTextVideoBufferSub : public etl::message<EventID::SetTextVideoBufferSub>
{
    uint16_t* textVideoBufferSub = nullptr;
};
} // namespace Event
