#pragma once
#include "Bounds.h"
#include "GLModel.h"
#include "String.h"

class Collider;

class Model
{
public:
	GLModel model;
	Bounds bounds;
	String defaultMaterial;
	Collider* collider;

	Model() : collider(nullptr) {}

	inline bool operator==(const Model &other) const { return model == other.model; }

	inline void Render() const { model.Render(); }
};
