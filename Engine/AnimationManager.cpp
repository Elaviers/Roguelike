#include "AnimationManager.hpp"

Animation* AnimationManager::_CreateResource(const String &name, const Buffer<byte> &data)
{
	Animation* anim = Animation::FromData(data);

	if (anim == nullptr)
		Debug::Error(CSTR("Could not load animation \"", name, '\"'));

	return anim;
}
