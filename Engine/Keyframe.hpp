#pragma once
#include "Maths.hpp"
#include "Quaternion.hpp"

enum class InterpolationType
{
	NONE,
	LINEAR
};

template <typename T>
struct Keyframe
{
	float time;
	InterpolationType interpolation;	//Interpolation to next keyframe

	T value;

	inline T Interpolate(const Keyframe& to, float t) const
	{
		if (interpolation == InterpolationType::LINEAR)
			return Maths::Lerp(value, to.value, (t - time) / (to.time - time));

		return value;
	}
};