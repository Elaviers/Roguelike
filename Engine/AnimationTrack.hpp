#pragma once
#include "Buffer.hpp"
#include "Keyframe.hpp"

template <typename T>
class AnimationTrack
{
private:
	Buffer<Keyframe<T>> _keyframes;

	int GetKeyIndex(float time) const
	{
		for (size_t i = 0; i < _keyframes.GetSize(); ++i)
		{
			if (_keyframes[i].time == time)
				return i;

			if (_keyframes[i].time > time)
			{
				if (i > 0)
					return i - 1;

				return 0;
			}
		}

		return -1;
	}

public:
	AnimationTrack() {}
	~AnimationTrack() {}

	inline const Keyframe<T>* GetKey(float time) const
	{
		int index = GetKeyIndex(time);

		if (index < 0)
			return nullptr;

		return &_keyframes[index];
	}

	const bool Evaluate(T &value, float time) const
	{
		int index = GetKeyIndex(time);

		if (index < 0)
			return false;

		const Keyframe<T>& keyframe = _keyframes[index];

		if (keyframe.interpolation == InterpolationType::LINEAR && index < _keyframes.GetSize() - 1)
		{
			value = keyframe.Interpolate(_keyframes[index + 1], time);
		}
		else
			value = keyframe.value;
		
		return true;
	}
};
