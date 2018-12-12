#pragma once
#include "PropertyWindow.h"

class Editor;
struct MouseData;

class Tool
{
protected:
	Editor &_owner;
	Tool(Editor &owner) : _owner(owner) {}

public:
	virtual ~Tool() {}

	virtual void Initialise() {}

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) { properties.Clear(); toolProperties.Clear(); }
	virtual void Deactivate() {}

	//Input
	virtual void Cancel() {}
	virtual void MouseMove(const MouseData&) {}
	virtual void MouseDown(const MouseData&) {}
	virtual void MouseUp(const MouseData&) {}
	virtual void KeySubmit() {}
	virtual void KeyDelete() {}

	virtual void Render() const {}
};

