#pragma once
#include "String.h"

struct RenderParam;

class Material
{
protected:
	Material() {}
	virtual ~Material() {}

public:
	virtual void FromString(const String &string) {}

	virtual void Apply(const RenderParam *param = nullptr) const = 0;
};
