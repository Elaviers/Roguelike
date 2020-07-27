#pragma once
#include "Colour.hpp"

#define SIZEOF_DEF virtual size_t SizeOf() const override { return sizeof(*this); }

class RenderQueue;

class DebugObject
{
private:
	float _timeRemaining;

	//Starts negative
	float _fadeTime;
	float _fadeSpeed;

protected:
	Colour _colour;
	float _lineWidth;

	/*
		time: how long to exist for
		fadeTime: how much time it will take for actor to fade out from 1 alpha
	*/
	DebugObject(float time, float fadeTime, const Colour& colour, float lineWidth) 
		: _timeRemaining(time), 
		_fadeTime(fadeTime - time),
		_fadeSpeed(fadeTime == 0 ? 0.f : 1.f / fadeTime),
		_colour(colour), 
		_lineWidth(lineWidth) 
	{}

public:
	virtual ~DebugObject() {}

	void Update(float deltaTime);

	virtual void Render(RenderQueue&) const = 0;

	bool ShouldBeRemoved() const { return _timeRemaining < 0.f; }

	virtual size_t SizeOf() const = 0;
};
