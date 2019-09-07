#pragma once
#include "Asset.hpp"
#include "AnimationTrack.hpp"
#include "Hashmap.hpp"
#include "String.hpp"

class Skeleton;

class Animation : public Asset
{
	Hashmap<String, AnimationTrack<Vector3>> _translationTracks;
	Hashmap<String, AnimationTrack<Quaternion>> _rotationTracks;
	Hashmap<String, AnimationTrack<Vector3>> _scalingTracks;

	virtual void _ReadData(BufferReader<byte>&) override;
	virtual void _WriteData(BufferWriter<byte>&) const override;

public:
	Animation() {}
	virtual ~Animation() {}

	static Animation* FromData(const Buffer<byte>&);

	void Evaluate(Skeleton& outSkeleton, float time) const;

	inline AnimationTrack<Vector3>& GetTranslationTrack(const String& joint) { return _translationTracks[joint]; }
	inline AnimationTrack<Quaternion>& GetRotationTrack(const String& joint) { return _rotationTracks[joint]; }
	inline AnimationTrack<Vector3>& GetScalingTrack(const String& joint) { return _scalingTracks[joint]; }
};
