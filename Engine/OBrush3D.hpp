#pragma once
#include "ORenderable.hpp"
#include <ELCore/Context.hpp>
#include <ELMaths/Volume.hpp>

class OBrush3D : public ORenderable
{
protected:
	Vector3 _point1;
	Vector3 _point2;

	VBox _volume;

	void _OnPointChanged();

	bool _updatingTransform;

protected:
	virtual void _InitBody(FixedBody& body) const override;
	virtual void _UpdateBody(FixedBody& body) const override;

	virtual void _OnTransformChanged() override;

	OBrush3D(World& world) : ORenderable(world), _updatingTransform(false) {}
	OBrush3D(const OBrush3D& other) : ORenderable(other), _point1(other._point1), _point2(other._point2), _updatingTransform(false) {}

public:
	WORLDOBJECT_VFUNCS(OBrush3D, EObjectID::BRUSH);
	virtual ~OBrush3D() {}

	void SetPoint1(const Vector3& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector3& position) { _point2 = position; _OnPointChanged(); }

	const Vector3& GetPoint1() const { return _point1; }
	const Vector3& GetPoint2() const { return _point2; }

	virtual void Render(RenderQueue&) const override;

	virtual const Volume* GetVolume() const override { return &_volume; }

	virtual void Read(ByteReader &buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter &buffer, ObjectIOContext& ctx) const override;

	virtual const PropertyCollection& GetProperties() override;
};
