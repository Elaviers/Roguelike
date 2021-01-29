#pragma once
#include "Tool.hpp"
#include <Engine/EEntityID.hpp>
#include <ELCore/PropertyCollection.hpp>
#include <ELCore/SharedPointer.hpp>

class Viewport;

class ToolEntity : public Tool
{
	byte _classID;

	EntityPointer _placement;
	bool _readyToPlace;

	const PropertyCollection& _GetProperties();

	void _SetClassID(const byte &id);
	byte _GetClassID() const { return _classID; }

	bool _ViewportCanPlace(const Viewport&) const;

public:
	ToolEntity(Editor &level) : Tool(level), _classID((byte)EEntityID::NONE), _placement(), _readyToPlace(false) {}
	virtual ~ToolEntity() {}

	virtual void Initialise() override;
	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;
	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;

	virtual void Render(RenderQueue&) const override;
};
