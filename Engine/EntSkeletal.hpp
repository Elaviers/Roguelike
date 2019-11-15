#pragma once
#include "EntRenderable.hpp"
#include "Animation.hpp"

class EntSkeletal : public EntRenderable
{

public:
	Entity_FUNCS(EntSkeletal, EntityIDS::SKELETAL);

	virtual void Render();

	void PlayAnimation(const Animation&);
};
