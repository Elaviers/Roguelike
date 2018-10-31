#pragma once
#include "GameObject.h"
#include "Engine.h"

class Renderable : public GameObject
{
private:
	const Model *_model;
	const Material *_material;

	bool _materialIsDefault;

public:
	Renderable() : _model(nullptr), _material(nullptr) {}
	virtual ~Renderable() {}

	inline void SetModel(const String &name) { if (Engine::modelManager) SetModel(Engine::modelManager->GetModel(name)); }
	inline void SetModel(const Model *model) { _model = model; if (model->defaultMaterial.GetLength() != 0) { SetMaterial(model->defaultMaterial); _materialIsDefault = true; } }

	inline void SetMaterial(const String &name) { if (Engine::materialManager) SetMaterial(Engine::materialManager->GetMaterial(name)); }
	inline void SetMaterial(const Material *material) { _material = material; _materialIsDefault = false; }

	inline bool MaterialIsDefault() const { return _materialIsDefault; }

	virtual void Render() const override;

	virtual void GetProperties(ObjectProperties&) override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;

	//File IO
	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const override;
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &strings) override;

	//Collision
	virtual const Collider* GetCollider() const override;

	//Other
	virtual Bounds GetBounds() const override 
	{
		if (_model) return Bounds(transform.Position() + _model->bounds.min, transform.Position() + _model->bounds.max);
		else return Bounds(transform.Position(), transform.Position());
	}
};
