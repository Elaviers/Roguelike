#pragma once
#include "GameObject.h"
#include "Engine.h"

class Brush2D : public GameObject
{
	Vector2 _point1;
	Vector2 _point2;
	
	const Material *_material;

	void _UpdateTransform();

public:
	float level;

	Brush2D() { transform.SetRotation(Vector3(-90.f, 0.f, 0.f)); }
	virtual ~Brush2D() {}

	virtual void GetProperties(ObjectProperties&) override;
	virtual void Render() const override;

	void SetPoint1(const Vector2 &position) { _point1 = position; }
	void SetPoint2(const Vector2 &position) { _point2 = position; _UpdateTransform(); }

	inline Vector2 GetPoint1() const { return _point1; }
	inline Vector2 GetPoint2() const { return _point2; }

	inline void SetMaterial(const Material *material) { _material = material; }
	void SetMaterial(const String &name) { if (Engine::materialManager)_material = Engine::materialManager->GetMaterial(name); }
	
	inline const Material* GetMaterial() const { return _material; }
	String GetMaterialName() const;

	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const override;
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &mstrings) override;
};
