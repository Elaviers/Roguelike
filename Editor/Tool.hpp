#pragma once
#include <ELGraphics/ERenderChannels.hpp>
#include <ELUI/Container.hpp>

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

	virtual void Activate(UIContainer &properties, UIContainer& toolProperties) { }
	virtual void Deactivate() {}

	//Input
	virtual void Cancel()						{}
	virtual void MouseMove(const MouseData&)	{}
	virtual void MouseDown(const MouseData&)	{}
	virtual void MouseUp(const MouseData&)		{}
	virtual void KeySubmit()					{}
	virtual void KeyDelete()					{}

	virtual void Render(RenderQueue&) const		{}
};

