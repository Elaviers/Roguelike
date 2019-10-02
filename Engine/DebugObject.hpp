#pragma once
#include "Colour.hpp"
#include "Vector4.hpp"

#define SIZEOF_DEF virtual size_t SizeOf() const override { return sizeof(*this); }

class DebugObject
{
private:
	float _timeRemaining;

protected:
	Colour _colour;

	DebugObject(float time, const Colour& colour) : _timeRemaining(time), _colour(colour) {}

	void _PreRender() const;

public:
	virtual ~DebugObject() {}

	void Update(float deltaTime);

	virtual void Render() const = 0;

	inline bool ShouldBeRemoved() const { return _timeRemaining < 0.f; }

	virtual size_t SizeOf() const = 0;
};
