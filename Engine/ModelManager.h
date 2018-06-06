#pragma once
#include "Map.h"
#include "ModelRenderer.h"
#include "String.h"

class ModelManager
{
private:
	Map<String, ModelRenderer> _models;

public:
	ModelManager();
	~ModelManager();

	void LoadModel(const char *filepath, const char *name);

	inline ModelRenderer* GetRenderer(const char *name) { return _models.Find(name); }
};
