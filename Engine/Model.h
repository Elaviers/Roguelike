#pragma once
#include "GLModel.h"
#include "String.h"

class Model
{
public:
	GLModel model;
	String defaultMaterial;

	inline bool operator==(const Model &other) const { return model == other.model; }

	inline void Render() const { model.Render(); }
};
