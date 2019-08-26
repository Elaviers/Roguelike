#pragma once
#include "GameObject.hpp"
#include "ColliderBox.hpp"
#include "Vector.hpp"

class ModelManager;

class ObjLight : public GameObject
{
	Vector3 _colour;
	float _radius;

	const static Vector3 _editorBoxExtent;
	const static ColliderBox _lightCollider;
public:
	GAMEOBJECT_FUNCS(ObjLight, ObjectIDS::LIGHT)

	ObjLight() : GameObject(FLAG_SAVEABLE), _radius(-1.f), _colour(1.f, 1.f, 1.f) { }
	virtual ~ObjLight() {}

	static bool drawLightSources;

	inline const Vector3& GetColour() const { return _colour; }
	inline float GetRadius() const { return _radius; }

	inline void SetColour(const Vector3 &colour) { _colour = colour; }
	inline void SetRadius(float radius) { _radius = radius; }

	void ToShader(int glArrayIndex);

	void Render(EnumRenderChannel) const override;

	virtual void GetCvars(CvarMap&) override;

	virtual Bounds GetBounds() const { return Bounds(_editorBoxExtent); }
	virtual const Collider* GetCollider() const { return &_lightCollider; }

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};
