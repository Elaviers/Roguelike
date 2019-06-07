#pragma once
#include "Tool.h"
#include <Engine/ObjBox.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/GameObject.hpp>

class ToolSelect : public Tool
{
	bool _placing;

	ObjBox _box;

	Buffer<GameObject*> _selectedObjects;

	GameObject *_focusedObject;
	float _dragOffsetX;
	float _dragOffsetY;

public:
	ToolSelect(Editor &level) : Tool(level), _placing(false), _focusedObject(nullptr), _dragOffsetX(0.f), _dragOffsetY(0.f) {}
	virtual ~ToolSelect() {}

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;
	virtual void KeyDelete() override;

	virtual void Render() const override;
};
