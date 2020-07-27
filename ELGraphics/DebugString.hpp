#pragma once
#include "DebugObject.hpp"
#include <ELCore/String.hpp>
#include <ELMaths/Transform.hpp>

class Font;

class DebugString : public DebugObject
{
	const Font *_font;
	Transform _transform;
	String _string;

public:
	DebugString(const Font &font, const Vector3 &position, float size, const String &string, float time, const Colour& colour, float fadeTime = 1.f) : 
		DebugObject(time, fadeTime, colour, 0.f), 
		_font(&font), 
		_string(string) 
	{ 
		_transform.SetPosition(position); 
		_transform.SetScale(Vector3(size, size, 1.f)); 
	}
	
	~DebugString() {}

	virtual void Render(RenderQueue&) const override;

	SIZEOF_DEF;
};
