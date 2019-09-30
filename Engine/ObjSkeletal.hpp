#pragma once
#include "ObjRenderable.hpp"
#include "Animation.hpp"

class ObjSkeletal : public ObjRenderable
{

public:
	GAMEOBJECT_FUNCS(ObjSkeletal, ObjectIDS::SKELETAL);

	virtual void Render();

	void PlayAnimation(const Animation&);
};
