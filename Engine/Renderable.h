#pragma once
#include "GameObject.h"
#include "GLModel.h"
#include "Material.h"
#include "MaterialManager.h"
#include "ModelManager.h"

class Renderable : public GameObject
{
private:
	const GLModel *_model;
	const Material *_material;

	static MaterialManager* _materialManager;
	static ModelManager* _modelManager;

public:
	Renderable() {}
	virtual ~Renderable() {}

	inline static void SetManagers(MaterialManager* materialManager, ModelManager* modelManager)
	{
		_materialManager = materialManager;
		_modelManager = modelManager;
	}

	inline void SetModel(const char *name) { if (_modelManager) _model = _modelManager->GetModel(name); }
	inline void SetModel(const GLModel *model) { _model = model; }

	inline void SetMaterial(const char *name) { if (_materialManager) _material = _materialManager->Get(name); }
	inline void SetMaterial(const Material *material) { _material = material; }

	virtual void Render() override;

	virtual void GetProperties(ObjectProperties&) override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;
	inline void SetModel(const String &name) { SetModel(name.GetData()); }
	inline void SetMaterial(const String &name) { SetMaterial(name.GetData()); }
};
