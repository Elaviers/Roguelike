#pragma once
#include "EntRenderable.hpp"
#include "Animation.hpp"

class EntSkeletal : public EntRenderable
{
	Buffer<Mat4> _skinningMatrices;

	SharedPointer<const Animation> _animation;
	float _currentTime;

	void _CMD_PlayAnimation(const Buffer<String>& args);

	virtual void _OnModelChanged() override;

public:
	Entity_FUNCS(EntSkeletal, EntityIDS::SKELETAL);

	EntSkeletal() : _currentTime(0.f) {}
	virtual ~EntSkeletal() {}

	virtual void Update(float deltaTime) override;
	virtual void Render(EnumRenderChannel) const override;

	virtual PropertyCollection& GetProperties() override;

	void PlayAnimation(const SharedPointer<const Animation>&);
};
