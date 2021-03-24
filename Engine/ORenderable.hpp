#pragma once
#include "WorldObject.hpp"
#include "ModelManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/Handle.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELPhys/Body.hpp>

class ORenderable : public WorldObject
{
private:
	RenderEntry _renderEntry;

	bool _static; //If static, render entry only calculated when object moves. Static objects are less efficient when they move frequently.

	void _P_InitFixedBody() const { if (_physics.IsValid()) _InitBody(*_physics); } //silly little helper method
	void _P_UpdateFixedBody() const { if (_physics.IsValid()) _UpdateBody(*_physics); } //dumb
	void _UpdateRenderEntry();

	float _age = 0.f; //probably temp?

	Handle<FixedBody> _physics;

protected:
	SharedPointer<const Model> _model;
	SharedPointer<const Material> _material;

	Colour _colour;

	bool _materialIsDefault;

	virtual void _InitBody(FixedBody& body) const {};
	virtual void _UpdateBody(FixedBody& body) const;

	virtual void _OnCreated() override;
	virtual void _OnTransformChanged() override;

	virtual void _OnMeshChanged() {}

	ORenderable(World& world);
	ORenderable(const ORenderable& other);

public:
	WORLDOBJECT_VFUNCS(ORenderable, EObjectID::RENDERABLE);

	virtual ~ORenderable() {}

	const Colour& GetColour() const { return _colour; }

	bool MaterialIsDefault() const { return _materialIsDefault; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	const SharedPointer<const Model>& GetModel() const { return _model; }

	void SetStatic(bool s) { _static = s; if (_static) _UpdateRenderEntry(); }

	void SetColour(const Colour& colour) { _colour = colour; _UpdateRenderEntry(); }
	
	void SetMaterial(const String &name, const Context& ctx) { SetMaterial(ctx.GetPtr<MaterialManager>()->Get(name, ctx)); }
	void SetMaterial(const SharedPointer<const Material>& material);

	void SetModel(const String& name, const Context& ctx) { SetModel(ctx.GetPtr<ModelManager>()->Get(name, ctx), ctx); }
	void SetModel(const SharedPointer<const Model>& model, const Context& ctx);

	virtual void Update(float deltaTime) override { _age += deltaTime; }
	virtual void Render(RenderQueue&) const override;

	virtual const Volume* GetVolume() const override;
	
	virtual const PropertyCollection& GetProperties() override;

	void SetCollisionEnabled(bool collisionEnabled);
	bool GetCollisionEnabled() const { return _physics.IsValid(); }

	//These are for properties really
	String GetModelName(const Context& ctx) const;
	String GetMaterialName(const Context& ctx) const;

	//File IO
	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;
};
