#include "MathManager.hpp"

float MathManager::tanDepreciated(float x)
{
    return M_PI_4 * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
}

q20_12_t MathManager::div(q20_12_t num, q20_12_t den)
{
    return q20_12_t::from_raw_value(divf32(num.raw_value(), den.raw_value()));
}

q20_12_t MathManager::sqrt(q20_12_t v)
{
    uint32_t raw = sqrtf32(static_cast<uint32_t>(v.raw_value()));
    return q20_12_t::from_raw_value(static_cast<int32_t>(raw));
}

q20_12_t MathManager::mod(q20_12_t num, q20_12_t den)
{
    return q20_12_t::from_raw_value(mod32(num.raw_value(), den.raw_value()));
}

q4_12_t MathManager::sin(angle16_t angle)
{
    return q4_12_t::from_raw_value(sinLerp(angle));
}

q4_12_t MathManager::cos(angle16_t angle)
{
    return q4_12_t::from_raw_value(cosLerp(angle));
}

q20_12_t MathManager::tan(angle16_t angle)
{
    return q20_12_t::from_raw_value(tanLerp(angle));
}

angle16_t MathManager::asin(q4_12_t ratio)
{
    return asinLerp(ratio.raw_value());
}

angle16_t MathManager::acos(q4_12_t ratio)
{
    return acosLerp(ratio.raw_value());
}

float MathManager::atan2(float x, float y)
{
    if (x >= 0)
    { // -pi/2 .. pi/2
        if (y >= 0)
        { // 0 .. pi/2
            if (y < x)
            { // 0 .. pi/4
                return tan(y / x);
            }
            else
            { // pi/4 .. pi/2
                return M_PI_2 - tan(x / y);
            }
        }
        else
        {
            if (-y < x)
            { // -pi/4 .. 0
                return tan(y / x);
            }
            else
            { // -pi/2 .. -pi/4
                return -M_PI_2 - tan(x / y);
            }
        }
    }
    else
    { // -pi..-pi/2, pi/2..pi
        if (y >= 0)
        { // pi/2 .. pi
            if (y < -x)
            { // pi*3/4 .. pi
                return tan(y / x) + M_PI;
            }
            else
            { // pi/2 .. pi*3/4
                return M_PI_2 - tan(x / y);
            }
        }
        else
        { // -pi .. -pi/2
            if (-y < -x)
            { // -pi .. -pi*3/4
                return tan(y / x) - M_PI;
            }
            else
            { // -pi*3/4 .. -pi/2
                return -M_PI_2 - tan(x / y);
            }
        }
    }
}
