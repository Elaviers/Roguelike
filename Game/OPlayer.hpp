#pragma once
#include <Engine/WorldObject.hpp>
#include <ELCore/SharedPointer.hpp>
#include <Engine/OCamera.hpp>
#include <Engine/OSkeletal.hpp>

class OPlayer : public WorldObject
{
	OSkeletal* _mesh;

	OCamera* _camera;

	Transform _rootCameraTransform;

	SharedPointer<const Texture> _targetTexture;
	Transform _targetTransform;

	Handle<PhysicsBody> _physics;

	void _Jump();


protected:
	virtual void _OnTransformChanged() override;
	
	OPlayer(World& world);
	OPlayer(const OPlayer& other);

public:
	WORLDOBJECT_VFUNCS(OPlayer, (EObjectID)100);

	virtual ~OPlayer() {}

	virtual void Update(float deltaTime) override;
	virtual void Render(RenderQueue&) const override;
};
