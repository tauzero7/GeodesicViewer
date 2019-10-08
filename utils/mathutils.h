/**
 * @file    mathutils.h
 * @author  Thomas Mueller
 *
 * @brief  Math utility functions.
 *
 * This file is part of GeodesicView.
 */
#ifndef GV_MATH_UTILS_H
#define GV_MATH_UTILS_H

constexpr double PI = 3.14159265358979323846;
constexpr float PI_F = 3.14159265f;

double rad_to_deg(double val);

double deg_to_rad(double val);

float clamp_f(float val, float vmin, float vmax);

#endif // GV_MATH_UTILS_H
