#pragma once
#include "ORenderable.hpp"
#include <ELMaths/Volume.hpp>

/*
	Todo: Allow on any axis
*/
class OBrush2D : public ORenderable
{
private:
	Handle<FixedBody> _physics;

protected:
	EAxis _axis;

	Vector2 _point1;
	Vector2 _point2;
	float _level;

	VBox _volume;

	virtual void _InitBody(FixedBody& body) const override;
	virtual void _UpdateBody(FixedBody& body) const override;

	virtual void _OnTransformChanged() override;
	void _OnPointChanged();

	bool _updatingTransform;

protected:
	OBrush2D(World& world);
	OBrush2D(const OBrush2D& other);

public:
	WORLDOBJECT_VFUNCS(OBrush2D, EObjectID::PLANE);

	virtual ~OBrush2D() {}

	void SetPoint1(const Vector2& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector2& position) { _point2 = position; _OnPointChanged(); }
	void SetLevel(const float& level) { _level = level; _OnPointChanged(); }

	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; }
	void SetMaterial(const String& name, const Context& ctx) { _material = ctx.GetPtr<MaterialManager>()->Get(name, ctx); }

	const Vector2& GetPoint1() const { return _point1; }
	const Vector2& GetPoint2() const { return _point2; }
	float GetLevel() const { return _level; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	String GetMaterialName(const Context& ctx) const
	{
		if (_material) return ctx.GetPtr<MaterialManager>()->FindNameOf(_material.Ptr());
		return "Unknown";
	}

	virtual void Render(RenderQueue&) const override;

	virtual const Volume* GetVolume() const override { return &_volume; }
	
	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;

	virtual const PropertyCollection& GetProperties() override;
};
