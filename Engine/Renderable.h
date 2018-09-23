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
	Renderable() {}
	virtual ~Renderable() {}

	inline void SetModel(const String &name) { if (Engine::modelManager) SetModel(Engine::modelManager->GetModel(name)); }
	inline void SetModel(const Model *model) { _model = model; if (model->defaultMaterial.GetLength() != 0) { SetMaterial(model->defaultMaterial); _materialIsDefault = true; } }

	inline void SetMaterial(const String &name) { if (Engine::materialManager) SetMaterial(Engine::materialManager->GetMaterial(name)); }
	inline void SetMaterial(const Material *material) { _material = material; _materialIsDefault = false; }

	virtual void Render() const override;

	virtual void GetProperties(ObjectProperties&) override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;

	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const override;
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &strings) override;

	inline bool MaterialIsDefault() const { return _materialIsDefault; }
};
