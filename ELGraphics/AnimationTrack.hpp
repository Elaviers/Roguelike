#pragma once
#include "Keyframe.hpp"
#include <ELCore/Buffer.hpp>
#include <ELSys/Debug.hpp>

template <typename T>
class AnimationTrack
{
	Buffer<Keyframe<T>> _keyframes;

	size_t GetKeyIndex(float time) const
	{
		for (size_t i = 0; i < _keyframes.GetSize(); ++i)
		{
			if (_keyframes[i].time > time)
			{
				if (i > 0)
					return i - 1;

				return 0;
			}
		}

		//We are past the last keyframe
		return _keyframes.GetSize() - 1;
	}

	static bool _TypesAreAlmostEqual(const T& a, const T& b);

public:
	AnimationTrack() {}
	~AnimationTrack() {}

	const Buffer<Keyframe<T>>& GetKeyframes() const { return _keyframes; }

	void AddKey(float time, const T& value, byte interp = INTERP_LINEAR)
	{
		if (_keyframes.GetSize() == 0)
			_keyframes.Add(Keyframe<T>(time, value, interp));
		else if (time > _keyframes.Last().time)
		{
			if (!_TypesAreAlmostEqual(value, _keyframes.Last().value))
				_keyframes.Add(Keyframe<T>(time, value, interp));
		}
		else
			Debug::PrintLine("ERROR: Right now you can't add keyframes that have lower time values than the last one in the buffer");
	}

	const Keyframe<T>* GetKey(float time) const
	{
		int index = GetKeyIndex(time);

		if (index < 0)
			return nullptr;

		return &_keyframes[index];
	}

	const bool Evaluate(T &value, float time) const
	{
		size_t index = GetKeyIndex(time);

		if (index < 0)
			return false;

		const Keyframe<T>& keyframe = _keyframes[index];

		//if (keyframe.interpolation == INTERP_LINEAR && index < _keyframes.GetSize() - 1)
		//{
		//	value = keyframe.Interpolate(_keyframes[index + 1], time);
		//}
		//else
			value = keyframe.value;
		
		return true;
	}
};
