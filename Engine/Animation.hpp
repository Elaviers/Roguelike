#pragma once
#include "Asset.hpp"
#include "AnimationTrack.hpp"
#include "Map.hpp"
#include "String.hpp"

class Skeleton;

class Animation : public Asset
{
	Map<String, AnimationTrack<Vector3>> _translationTracks;
	Map<String, AnimationTrack<Quaternion>> _rotationTracks;
	Map<String, AnimationTrack<Vector3>> _scalingTracks;

	virtual void _ReadData(BufferReader<byte>&) override;
	virtual void _WriteData(BufferWriter<byte>&) const override;

public:
	Animation() {}
	virtual ~Animation() {}

	static Animation* FromData(const Buffer<byte>&);

	void Evaluate(Skeleton& outSkeleton, float time) const;

	inline AnimationTrack<Vector3>* GetTranslationTrack(const String& joint) { return _translationTracks.Get(joint); }
	inline AnimationTrack<Quaternion>* GetRotationTrack(const String& joint) { return _rotationTracks.Get(joint); }
	inline AnimationTrack<Vector3>* GetScalingTrack(const String& joint) { return _scalingTracks.Get(joint); }
};
