/**
 * @file    mathutils.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "utils/mathutils.h"
#include <cmath>

double rad_to_deg(double val) {
    return val / PI * 180.0;
}

double deg_to_rad(double val) {
    return val / 180.0 * PI;
}

float clamp_f(float val, float vmin, float vmax) {
    return (val < vmin ? vmin : (val > vmax ? vmax : val));
}
