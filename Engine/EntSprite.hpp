#pragma once
#include "Entity.hpp"
#include "ColliderSphere.hpp"
#include "Colour.hpp"
#include "MaterialManager.hpp"
#include "Material_Sprite.hpp"

class EntSprite : public Entity
{
	SharedPointer<const MaterialSprite> _material;
	float _size;
	Colour _colour;

	ColliderSphere _editorCollider;

	bool _fixedYaw;

public:
	Entity_FUNCS(EntSprite, EntityID::SPRITE)

	EntSprite(const MaterialSprite *material = nullptr, float size = 1.f) : 
		_material(material), 
		_size(size),
		_colour(1.f, 1.f, 1.f),
		_editorCollider(CollisionChannels::EDITOR, size / 2.f),
		_fixedYaw(true)
	{}
	virtual ~EntSprite() {}

	float GetSize() const { return _size; }
	const Colour& GetColour() const { return _colour; }

	void SetSize(float size) { _size = size; _editorCollider.SetRadius(size / 2.f); }
	void SetColour(const Colour& colour) { _colour = colour; }

	virtual void Render(RenderChannels) const override;

	virtual Bounds GetBounds() const override { return Bounds(_size / 2.f); }
	virtual const Collider* GetCollider() const override { return &_editorCollider; }

	virtual const PropertyCollection& GetProperties() override;

	SharedPointer<const MaterialSprite> GetMaterial() const { return _material; }

	String GetMaterialName() const
	{
		return Engine::Instance().pMaterialManager->FindNameOf(_material.Ptr());
	}

	void SetMaterialName(const String& name)
	{
		_material = Engine::Instance().pMaterialManager->Get(name).Cast<const MaterialSprite>();
	}

	void SetMaterial(const SharedPointer<const MaterialSprite>& material)
	{
		_material = material;
	}

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};

