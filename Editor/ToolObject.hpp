#pragma once
#include "Tool.hpp"
#include <Engine/EObjectID.hpp>
#include <Engine/WorldObject.hpp>
#include <ELCore/PropertyCollection.hpp>
#include <ELCore/SharedPointer.hpp>

class Viewport;

class ToolObject : public Tool
{
	EObjectID _classID;

	SharedPointer<WorldObject> _placement;
	bool _readyToPlace;

	const PropertyCollection& _GetProperties();

	void _SetClassID(const byte &id);
	byte _GetClassID() const { return (byte)_classID; }

	bool _ViewportCanPlace(const Viewport&) const;

public:
	ToolObject(Editor &level) : Tool(level), _classID(EObjectID::NONE), _placement(), _readyToPlace(false) {}
	virtual ~ToolObject() {}

	virtual void Initialise() override;
	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;
	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;

	virtual void Render(RenderQueue&) const override;
};
