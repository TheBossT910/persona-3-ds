/**
 * @file MathManger.hpp
 * @brief Manager for hardware specific math functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <aegis/manager.hpp>
#include <aegis/types.hpp>
#include <math.h>
#include <nds/arm9/math.h>
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
    * @brief DEPRECITATED, will be replaced with coming Libnds version
    *  Computes the tangent from an angles using an approximation.
    *
    * @details Function courtesy of https://yal.cc/fast-atan2/.
    *
    * @param x The angle in radians.
    * @return The tangent of the angle.
    * @todo Replace with tanLerp?
    *
    * @author Vadym Diachenko (yellowafterlife)
    */
    float tanDepreciated(float r);

    // TODO: replace with native atan2 solution in upcoming BlocksDS update
    /**
     * @brief Computes the arctangent from two angles using an approximation.
     *
     * @details Function courtesy of https://yal.cc/fast-atan2/.
     * @note Does not handle infinities or NaNs. Has a limited input range (±45deg).
     *
     * @param y Point 2.
     * @param x Point 1.
     * @return The arctangent angle of the two points.
     *
     * @author Vadym Diachenko (yellowafterlife)
     */
    float atan2(float y, float x);

    /**
     * @brief Divides two Q20.12 values.
     *
     * @param num Numerator.
     * @param den Denominator.
     * @return The quotient in Q20.12.
     */
    q20_12_t div(q20_12_t num, q20_12_t den);

    /**
     * @brief Computes the square root of a Q20.12 value.
     *
     * @param v Value to root.
     * @return The square root in Q20.12.
     */
    q20_12_t sqrt(q20_12_t v);

    /**
     * @brief Computes the modulo of two Q20.12 values.
     *
     * @param num Numerator.
     * @param den Denominator.
     * @return The remainder in Q20.12.
     */
    q20_12_t mod(q20_12_t num, q20_12_t den);

    /**
     * @brief Converts an angle in radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The equivalent cyclic angle (full turn = 1<<15).
     */
    angle16_t radiansToAngle(q20_12_t radians);

    /**
     * @brief Converts a cyclic angle to radians (Q20.12).
     *
     * @param angle Cyclic angle (full turn = 1<<15).
     * @return The equivalent angle in radians, as a Q20.12 fixed-point value.
     */
    q20_12_t angleToRadians(angle16_t angle);

    /**
     * @brief Sine, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The sine in Q4.12.
     */
    q4_12_t sin(q20_12_t radians);

    /**
     * @brief Cosine, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The cosine in Q4.12.
     */
    q4_12_t cos(q20_12_t radians);

    /**
     * @brief Tangent, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The tangent in Q20.12.
     */
    q20_12_t tan(q20_12_t radians);

    /**
     * @brief Computes the arcsine of a ratio.
     *
     * @param ratio Ratio in Q4.12.
     * @return The cyclic angle whose sine is ratio.
     */
    angle16_t asin(q4_12_t ratio);

    /**
     * @brief Computes the arccosine of a ratio.
     *
     * @param ratio Ratio in Q4.12.
     * @return The cyclic angle whose cosine is ratio.
     */
    angle16_t acos(q4_12_t ratio);

    /**
     * @brief Computes the dot product of two Q20.12 3D vectors.
     *
     * @param x1 X component of the first vector.
     * @param y1 Y component of the first vector.
     * @param z1 Z component of the first vector.
     * @param x2 X component of the second vector.
     * @param y2 Y component of the second vector.
     * @param z2 Z component of the second vector.
     * @return The dot product in Q20.12.
     */
    q20_12_t dot(q20_12_t x1, q20_12_t y1, q20_12_t z1, q20_12_t x2, q20_12_t y2, q20_12_t z2);

    /**
     * @brief Computes the squared length of a Q20.12 3D vector.
     *
     * @param x X component.
     * @param y Y component.
     * @param z Z component.
     * @return The squared length in Q20.12.
     */
    q20_12_t lengthSq(q20_12_t x, q20_12_t y, q20_12_t z);

    /**
     * @brief Computes the length of a Q20.12 3D vector.
     *
     * @param x X component.
     * @param y Y component.
     * @param z Z component.
     * @return The length in Q20.12.
     */
    q20_12_t length(q20_12_t x, q20_12_t y, q20_12_t z);

    /**
     * @brief Normalizes a Q20.12 3D vector in place.
     *
     * @param x X component, updated in place.
     * @param y Y component, updated in place.
     * @param z Z component, updated in place.
     * @return Sets x, y, z to 0,0,0 if length is zero (via out parameters).
     */
    void normalize(q20_12_t& x, q20_12_t& y, q20_12_t& z);

  private:
    friend class Singleton<MathManager>;
    MathManager() = default;
};
