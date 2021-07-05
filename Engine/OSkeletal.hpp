#pragma once
#include "ORenderable.hpp"
#include <ELGraphics/Animation.hpp>

class OSkeletal : public ORenderable
{
	Buffer<Matrix4> _skinningMatrices;

	SharedPointer<const Animation> _animation;
	float _currentTime;

	void _CMD_PlayAnimation(const Array<String>& args, const Context&);

	virtual void _OnMeshChanged() override;

protected:
	OSkeletal(World& world) : ORenderable(world), _currentTime(0.f) {}

public:
	WORLDOBJECT_VFUNCS(OSkeletal, EObjectID::SKELETAL);

	virtual ~OSkeletal() {}

	virtual void Update(float deltaTime) override;
	virtual void Render(RenderQueue&) const override;

	virtual PropertyCollection& GetProperties() override;

	void PlayAnimation(const SharedPointer<const Animation>&);
};
