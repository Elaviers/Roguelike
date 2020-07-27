#pragma once
#include "EntRenderable.hpp"
#include <ELGraphics/Animation.hpp>

class EntSkeletal : public EntRenderable
{
	Buffer<Matrix4> _skinningMatrices;

	SharedPointer<const Animation> _animation;
	float _currentTime;

	void _CMD_PlayAnimation(const Buffer<String>& args, const Context&);

	virtual void _OnMeshChanged() override;

public:
	Entity_FUNCS(EntSkeletal, EEntityID::SKELETAL);

	EntSkeletal() : _currentTime(0.f) {}
	virtual ~EntSkeletal() {}

	virtual void Update(float deltaTime) override;
	virtual void Render(RenderQueue&) const override;

	virtual PropertyCollection& GetProperties() override;

	void PlayAnimation(const SharedPointer<const Animation>&);
};
