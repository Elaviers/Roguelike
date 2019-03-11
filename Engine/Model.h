#pragma once
#include "Bounds.h"
#include "CvarMap.h"
#include "GLModel.h"
#include "IO.h"	//For ModelData (silly)
#include "String.h"

class Collider;

class Model
{
protected:
	CvarMap _cvars;
	
	ModelData _data;

	void _CMD_model(const Buffer<String> &args);
	void _CMD_collision(const Buffer<String> &args);

public:
	GLModel model;
	Bounds bounds;
	String defaultMaterial;
	Collider* collider;

	Model() : collider(nullptr) { 
		_cvars.CreateVar("model", CommandPtr(this, &Model::_CMD_model));
		_cvars.CreateVar("collision", CommandPtr(this, &Model::_CMD_collision));
		_cvars.Add("material", defaultMaterial); }

	inline bool operator==(const Model &other) const { return model == other.model; }

	inline void Render() const { model.Render(); }

	void FromString(const String&);
};
