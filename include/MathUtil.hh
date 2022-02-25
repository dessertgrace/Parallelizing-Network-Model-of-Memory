/**
 * @file Math.hh
 *
 * Miscellaneous math utilities
 *
 * Author: Peter Helfer
 * Date: 2014-12-16
 */

#ifndef MATH_UTIL_HH
#define MATH_UTIL_HH

#include <math.h>
#include <vector>
using std::vector;

#include "Trace.hh"

namespace MathUtil {
    /**
     * Asigmoid function - grows from 0 to 1 as x grows from -inf to +inf
     * @param x The variable
     * @param k Higher value gives steeper slope
     * @param x_half The x-value where the asigmoid crosses y=0.5
     *
     * To concentrate almost all the action in the [0,1] interval,
     * try slew=10, x_half=0.5.
     */
    inline double asigmoid(double x, double k = 1.0, double x_half = 0.5)
    {
        return 1.0 / (1.0 + exp(-k * (x - x_half)));
    }

    /**
     * a * (1.0 - exp(k * x), grows from (0, 0) towards an asymptote.
     * @param x The variable
     * @param k Greater value for faster growth
     * @param a The asymptote
     *
     * With the default values, most of the action is in (0,0)-(1,1)
     */
    inline double asymptote(double x, double a = 1.0, double k = 5.0)
    {
        return a * (1.0 - exp(-k * x));
    }
}
#endif
