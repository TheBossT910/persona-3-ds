/**
 * @file MathManger.hpp
 * @brief Manager for hardware specific math functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <aegis/manager.hpp>
#include <math.h>
#include <nds/arm9/trig_lut.h>

class MathManager : public ae::Manager, public ae::Singleton<MathManager>
{
  public:
    void Init() override
    {
    }

    void Process() override
    {
    }

    void Shutdown() override
    {
    }

    /**
     * @brief Computes the sine of an angle using a hardware-optimized lookup table.
     *
     * @param r The angle in radians.
     * @return The sine of the angle, ranging from -1.0f to 1.0f.
     */
    float sin(float r);

    /**
     * @brief Computes the cosine of an angle using a hardware-optimized lookup table.
     *
     * @param r The angle in radians.
     * @return The cosine of the angle, ranging from -1.0f to 1.0f.
     */
    float cos(float r);

  private:
    friend class Singleton<MathManager>;
    MathManager() = default;

    const float RAD_TO_LIBNDS = 32768.0f / (2.0f * 3.14159265f);
};
