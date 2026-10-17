#ifndef Q24_H
#define Q24_H

#include <stdint.h>
#include <limits.h>

/*------------------------------------------------------------
 * Q8.24 Fixed Point Type
 *
 * Range:
 *     -128.0 to +127.99999994
 *
 * Resolution:
 *     1 / 2^24 = 5.96e-8
 *-----------------------------------------------------------*/

typedef int32_t q24_t;
typedef int64_t q24_acc_t;

#define Q24_SHIFT     (24)

/*------------------------------------------------------------
 * Common Constants
 *-----------------------------------------------------------*/

#define Q24_ZERO      ((q24_t)0x00000000)
#define Q24_ONE       ((q24_t)0x01000000)
#define Q24_NEG_ONE  ((q24_t)0xFF000000)

#define Q24_HALF      ((q24_t)0x00800000)
#define Q24_QUARTER   ((q24_t)0x00400000)
#define Q24_TWO       ((q24_t)0x02000000)

#define Q24_MAX       ((q24_t)INT32_MAX)
#define Q24_MIN       ((q24_t)INT32_MIN)

/*------------------------------------------------------------
 * Literal Macro
 *
 * Example:
 *     q24_t gain = Q24(0.70710678f);
 *-----------------------------------------------------------*/

#define Q24(x) ((q24_t)((x) * 16777216.0f))

/*------------------------------------------------------------
 * Conversions
 *-----------------------------------------------------------*/

static inline q24_t q24_from_int(int32_t x)
{
    return (q24_t)(x << Q24_SHIFT);
}

static inline int32_t q24_to_int(q24_t x)
{
    return x >> Q24_SHIFT;
}


static inline uint32_t q24_to_dac12(q24_t x)
{
    int32_t shifted = x + Q24_ONE;

    if (shifted < 0)
        return 0;

    if (shifted >= (2 * Q24_ONE))
        return 4095;

    return (uint32_t)(shifted >> 13);
}


static inline q24_t q24_from_float(float x)
{
    return (q24_t)(x * 16777216.0f);
}

static inline float q24_to_float(q24_t x)
{
    return (float)x / 16777216.0f;
}

/*------------------------------------------------------------
 * Basic Arithmetic
 *-----------------------------------------------------------*/

static inline q24_t q24_add(q24_t a, q24_t b)
{
    return a + b;
}

static inline q24_t q24_sub(q24_t a, q24_t b)
{
    return a - b;
}

static inline q24_t q24_neg(q24_t x)
{
    return -x;
}

/*------------------------------------------------------------
 * Multiply
 *-----------------------------------------------------------*/

static inline q24_t q24_mul(q24_t a, q24_t b)
{
    return (q24_t)(((int64_t)a * b) >> Q24_SHIFT);
}

/* Rounded Multiply */

static inline q24_t q24_mul_rnd(q24_t a, q24_t b)
{
    int64_t tmp = (int64_t)a * b;

    tmp += (tmp >= 0)
        ? (1LL << (Q24_SHIFT - 1))
        : -(1LL << (Q24_SHIFT - 1));

    return (q24_t)(tmp >> Q24_SHIFT);
}

/*------------------------------------------------------------
 * Division
 *-----------------------------------------------------------*/

static inline q24_t q24_div(q24_t a, q24_t b)
{
    return (q24_t)(((int64_t)a << Q24_SHIFT) / b);
}

/*------------------------------------------------------------
 * Magnitude Helpers
 *-----------------------------------------------------------*/

static inline q24_t q24_abs(q24_t x)
{
    return (x < 0) ? -x : x;
}

/*------------------------------------------------------------
 * Saturation
 *-----------------------------------------------------------*/

static inline q24_t q24_sat(int64_t x)
{
    if (x > INT32_MAX)
    {
        return INT32_MAX;
    }

    if (x < INT32_MIN)
    {
        return INT32_MIN;
    }

    return (q24_t)x;
}

static inline q24_t q24_add_sat(q24_t a, q24_t b)
{
    return q24_sat((int64_t)a + b);
}

static inline q24_t q24_sub_sat(q24_t a, q24_t b)
{
    return q24_sat((int64_t)a - b);
}

/*------------------------------------------------------------
 * Accumulator Helpers
 *-----------------------------------------------------------*/

/* Accumulate unshifted products in a 64-bit accumulator */

static inline void q24_mac(q24_acc_t *acc,
                           q24_t a,
                           q24_t b)
{
    *acc += (q24_acc_t)a * b;
}

/* Convert accumulator to Q8.24 with rounding */

static inline q24_t q24_acc_to_q24(q24_acc_t acc)
{
    acc += (1LL << 23);

    return (q24_t)(acc >> Q24_SHIFT);
}

#endif /* Q24_H */