#pragma once
#define DEGS_PER_RAD 57.295779513082320876798154814105f
#define RADS_PER_DEG 0.01745329251994329576923690768489f

float SquareRoot(float x);

float Sine(float radians);
float Cosine(float radians);
float Tangent(float radians);

inline float SineDegrees(float degrees) { return Sine(RADS_PER_DEG * degrees); };
inline float CosineDegrees(float degrees) { return Cosine(RADS_PER_DEG * degrees); };
inline float TangentDegrees(float degrees) { return Tangent(RADS_PER_DEG * degrees); };
