#pragma once
#include "Buffer.h"
#include "DebugObject.h"
#include "Utilities.h"

class Font;
class String;

class DebugManager
{
	Buffer<DebugObject*> _worldObjects;
	Buffer<DebugObject*> _screenObjects;

	const Font *_font;

public:
	DebugManager() {}
	~DebugManager() {}

	inline void SetFont(const Font &font) { _font = &font; }

	void Update(float deltaTime);

	void RenderWorld() const;
	void RenderScreen() const;

	inline void AddToWorld(const DebugObject &obj) { _worldObjects.Add(Utilities::CopyOf(obj, obj.SizeOf())); }
	inline void AddToScreen(const DebugObject &obj) { _worldObjects.Add(Utilities::CopyOf(obj, obj.SizeOf())); }

	void AddString(const String &string);
};
