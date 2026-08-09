#include "MathManager.hpp"

float MathManager::sin(float r)
{
    return sinLerp((s16)(r * RAD_TO_LIBNDS)) / 4096.0f;
}

float MathManager::cos(float r)
{
    return cosLerp((s16)(r * RAD_TO_LIBNDS)) / 4096.0f;
}

float MathManager::tan(float x)
{
    return M_PI_4 * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
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
