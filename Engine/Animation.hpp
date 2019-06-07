#pragma once
#include "AnimationTrack.hpp"
#include "Map.hpp"
#include "String.hpp"

class Skeleton;

class Animation
{
	Map<String, AnimationTrack<Vector3>> _translationTracks;
	Map<String, AnimationTrack<Quaternion>> _rotationTracks;
	Map<String, AnimationTrack<Vector3>> _scalingTracks;

public:
	Animation() {}
	~Animation() {}

	void Evaluate(Skeleton& outSkeleton, float time) const;
};
