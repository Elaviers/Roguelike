#include "AnimationManager.hpp"

Animation* AnimationManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& ext, const Context& ctx)
{
	Animation* anim = Animation::FromData(data);

	if (anim == nullptr)
		Debug::Error(CSTR("Could not load animation \"", name, '\"'));

	return anim;
}
