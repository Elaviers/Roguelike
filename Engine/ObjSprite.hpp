#pragma once
#include "GameObject.hpp"
#include "ColliderSphere.hpp"
#include "Colour.hpp"
#include "MaterialManager.hpp"
#include "Material_Sprite.hpp"

class ObjSprite : public GameObject
{
	const MaterialSprite* _material;
	float _size;
	Colour _colour;

	ColliderSphere _editorCollider;

	bool _fixedYaw;

public:
	GAMEOBJECT_FUNCS(ObjSprite, ObjectIDS::SPRITE)

	ObjSprite(const MaterialSprite *material = nullptr, float size = 1.f) : 
		_material(material), 
		_size(size),
		_colour(1.f, 1.f, 1.f),
		_editorCollider(COLL_EDITOR, size / 2.f),
		_fixedYaw(true)
	{}
	virtual ~ObjSprite() {}

	inline float GetSize() const { return _size; }
	inline const Colour& GetColour() const { return _colour; }

	inline void SetSize(float size) { _size = size; _editorCollider.SetRadius(size / 2.f); }
	inline void SetColour(const Colour& colour) { _colour = colour; }

	virtual void Render(EnumRenderChannel) const override;

	virtual Bounds GetBounds() const override { return Bounds(_size / 2.f); }
	virtual const Collider* GetCollider() const override { return &_editorCollider; }

	virtual const PropertyCollection& GetProperties() override;

	inline const MaterialSprite* GetMaterial() const { return _material; }

	String GetMaterialName() const
	{
		return Engine::Instance().pMaterialManager->FindNameOf(_material);
	}

	void SetMaterialName(const String& name)
	{
		_material = dynamic_cast<MaterialSprite*>(Engine::Instance().pMaterialManager->Get(name));
	}

	inline void SetMaterial(const MaterialSprite* material)
	{
		_material = material;
	}

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};

