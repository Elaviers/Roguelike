#pragma once
#include "DebugObject.h"
#include "String.h"
#include "Transform.h"

class Font;

class DebugString : public DebugObject
{
	const Font *_font;
	Transform _transform;
	String _string;

public:
	DebugString(const Font &font, const Vector3 &position, float size, const String &string, float time, Vector4 colour) : DebugObject(time, colour), _font(&font), _string(string) { _transform.SetPosition(position); _transform.SetScale(Vector3(size, size, 1.f)); }
	~DebugString() {}

	virtual void Render() const override;

	SIZEOF_DEF;
};
