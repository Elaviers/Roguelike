#pragma once
#include "Vector4.h"

#define SIZEOF_DEF virtual size_t SizeOf() const override { return sizeof(*this); }

class DebugObject
{
private:
	float _timeRemaining;

protected:
	Vector4 _colour;

	DebugObject(float time, Vector4 colour) : _timeRemaining(time), _colour(colour) {}
	virtual ~DebugObject() {}

	void _PreRender() const;

public:
	void Update(float deltaTime);

	virtual void Render() const = 0;

	inline bool ShouldBeRemoved() const { return _timeRemaining < 0.f; }

	virtual size_t SizeOf() const = 0;
};
