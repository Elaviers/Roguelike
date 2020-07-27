#pragma once
#include "DebugObject.hpp"
#include <ELCore/Buffer.hpp>
#include <ELCore/Utilities.hpp>

class Font;
class String;

class DebugManager
{
	Buffer<DebugObject*> _worldObjects;
	Buffer<DebugObject*> _screenObjects;

	const Font *_font;

public:
	DebugManager() : _font(nullptr) {}
	~DebugManager() {}

	void SetFont(const Font &font) { _font = &font; }

	void Update(float deltaTime);

	void RenderWorld(RenderQueue&) const;
	void RenderScreen(RenderQueue&) const;

	void AddToWorld(const DebugObject &obj)	{ _worldObjects.Add(Utilities::CopyOf(obj, obj.SizeOf())); }
	void AddToScreen(const DebugObject &obj) { _screenObjects.Add(Utilities::CopyOf(obj, obj.SizeOf())); }

	void AddString(const String &string);
};
