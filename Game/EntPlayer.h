#pragma once
#include <Engine/Entity.hpp>
#include <Engine/EntSkeletal.hpp>

class EntPlayer : public Entity
{
	EntSkeletal _mesh;

public:
	Entity_FUNCS(EntPlayer, 100);

	EntPlayer();
	virtual ~EntPlayer() {}

	void Update(float deltaTime);
};
