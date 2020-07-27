#pragma once
#include <ELMaths/Maths.hpp>
#include <ELMaths/Quaternion.hpp>

enum EInterpolationType
{
	INTERP_NONE = 0,
	INTERP_LINEAR = 1
};

template <typename T>
struct Keyframe
{
	float time;
	byte interpolation;	//Interpolation to next keyframe

	T value;
	
	Keyframe() : time(-1.f), interpolation(INTERP_NONE) {}
	Keyframe(float time, const T& value, byte interp = INTERP_LINEAR) : time(time), value(value), interpolation(interp) {}

	T Interpolate(const Keyframe& to, float t) const
	{
		if (interpolation == INTERP_LINEAR)
			return Maths::Lerp(value, to.value, (t - time) / (to.time - time));

		return value;
	}
};