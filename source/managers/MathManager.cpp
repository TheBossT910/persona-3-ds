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

angle16_t MathManager::radiansToAngle(q20_12_t radians)
{
    return static_cast<angle16_t>((static_cast<int32_t>(radians.raw_value()) * 5215) / 4096);
}

q20_12_t MathManager::angleToRadians(angle16_t angle)
{
    int32_t raw_rad = (static_cast<int32_t>(angle) * 51472) >> 16;
    return q20_12_t::from_raw_value(raw_rad);
}

q4_12_t MathManager::sin(q20_12_t radians)
{
    return q4_12_t::from_raw_value(sinLerp(radiansToAngle(radians)));
}

q4_12_t MathManager::cos(q20_12_t radians)
{
    return q4_12_t::from_raw_value(cosLerp(radiansToAngle(radians)));
}

q20_12_t MathManager::tan(q20_12_t radians)
{
    return q20_12_t::from_raw_value(tanLerp(radiansToAngle(radians)));
}

angle16_t MathManager::asin(q4_12_t ratio)
{
    return asinLerp(ratio.raw_value());
}

angle16_t MathManager::acos(q4_12_t ratio)
{
    return acosLerp(ratio.raw_value());
}

q20_12_t MathManager::dot(q20_12_t x1, q20_12_t y1, q20_12_t z1, q20_12_t x2, q20_12_t y2, q20_12_t z2)
{
    // libnds dotf32 handles the 64-bit accumulation internally and returns Q20.12
    s32 vec1[3] = {x1.raw_value(), y1.raw_value(), z1.raw_value()};
    s32 vec2[3] = {x2.raw_value(), y2.raw_value(), z2.raw_value()};
    return q20_12_t::from_raw_value(dotf32(vec1, vec2));
}

q20_12_t MathManager::lengthSq(q20_12_t x, q20_12_t y, q20_12_t z)
{
    return dot(x, y, z, x, y, z);
}

q20_12_t MathManager::length(q20_12_t x, q20_12_t y, q20_12_t z)
{
    return q20_12_t::from_raw_value(sqrtf32(lengthSq(x, y, z).raw_value()));
}

void MathManager::normalize(q20_12_t& x, q20_12_t& y, q20_12_t& z)
{
    s32 vec[3] = {x.raw_value(), y.raw_value(), z.raw_value()};

    normalizef32(vec);

    x = q20_12_t::from_raw_value(vec[0]);
    y = q20_12_t::from_raw_value(vec[1]);
    z = q20_12_t::from_raw_value(vec[2]);
}

float MathManager::atan2(float x, float y)
{
    if (x >= 0)
    { // -pi/2 .. pi/2
        if (y >= 0)
        { // 0 .. pi/2
            if (y < x)
            { // 0 .. pi/4
                return tanDepreciated(y / x);
            }
            else
            { // pi/4 .. pi/2
                return M_PI_2 - tanDepreciated(x / y);
            }
        }
        else
        {
            if (-y < x)
            { // -pi/4 .. 0
                return tanDepreciated(y / x);
            }
            else
            { // -pi/2 .. -pi/4
                return -M_PI_2 - tanDepreciated(x / y);
            }
        }
    }
    else
    { // -pi..-pi/2, pi/2..pi
        if (y >= 0)
        { // pi/2 .. pi
            if (y < -x)
            { // pi*3/4 .. pi
                return tanDepreciated(y / x) + M_PI;
            }
            else
            { // pi/2 .. pi*3/4
                return M_PI_2 - tanDepreciated(x / y);
            }
        }
        else
        { // -pi .. -pi/2
            if (-y < -x)
            { // -pi .. -pi*3/4
                return tanDepreciated(y / x) - M_PI;
            }
            else
            { // -pi*3/4 .. -pi/2
                return -M_PI_2 - tanDepreciated(x / y);
            }
        }
    }
}
