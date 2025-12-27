#pragma once

//constexpr float PI = 3.14;
#ifndef PI
#define PI		(3.14159265359f)
#endif

constexpr float DEGREE_TO_RADIANS = PI / 180.0f;
constexpr float RADIANS_TO_DEGREE = 180.f / PI;
constexpr float EPSILON = 0.000001f;