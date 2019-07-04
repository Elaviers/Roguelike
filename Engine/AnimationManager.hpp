#pragma once
#include "Animation.hpp"
#include "AssetManager.hpp"

class AnimationManager : public AssetManager<Animation>
{
	virtual Animation* _CreateResource(const String& name, const Buffer<byte>& data) override;

public:
	AnimationManager() : AssetManager(".anim", ".txt") {}
	virtual ~AnimationManager() {}
};
