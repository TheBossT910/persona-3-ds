#include "MathManager.hpp"

float MathManager::sin(float r)
{
    return sinLerp((s16)(r * RAD_TO_LIBNDS)) / 4096.0f;
}

float MathManager::cos(float r)
{
    return cosLerp((s16)(r * RAD_TO_LIBNDS)) / 4096.0f;
}
