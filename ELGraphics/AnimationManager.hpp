#pragma once
#include <ELSys/AssetManager.hpp>
#include "Animation.hpp"

class AnimationManager : public AssetManager<Animation>
{
	virtual Animation* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx) override;

public:
	AnimationManager() : AssetManager({ ".anim" }) {}
	virtual ~AnimationManager() {}
};
