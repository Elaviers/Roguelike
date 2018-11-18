#pragma once
#include "Tool.h"
#include <Engine/Box.h>
#include <Engine/Buffer.h>
#include <Engine/GameObject.h>

class ToolSelect : public Tool
{
	bool _placing;

	Box _box;

	Buffer<GameObject*> _selectedObjects;

	GameObject *_focusedObject;
	float _dragOffsetX;
	float _dragOffsetY;

public:
	ToolSelect(Editor &level) : Tool(level) {}
	virtual ~ToolSelect() {}

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;
	virtual void KeyDelete() override;

	virtual void Render() const override;
};

