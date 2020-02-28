#pragma once
#include "UIElement.hpp"
#include "UILabel.hpp"
#include "UIPanel.hpp"
#include "UIColour.hpp"
#include "Event.hpp"

class UIButton : public UIElement
{
protected:
	bool _hover;
	bool _hold;

	UIColour _colour;
	UIColour _colourHold;
	UIColour _colourHover;

	UIPanel _panel;

	virtual void _OnBoundsChanged() override
	{
		_panel.UpdateAbsoluteBounds();
	}

public:
	Event<UIButton&> onPressed;

	UIButton(UIElement* parent = nullptr) : UIElement(parent), _hover(false), _hold(false), _panel(this) { }

	virtual ~UIButton() {}

	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const UIColour& GetColour() const { return _colour; }
	const UIColour& GetColourHold() const { return _colourHold; }
	const UIColour& GetColourHover() const { return _colourHover; }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }

	UIButton& SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); return *this; }
	UIButton& SetColour(const UIColour&colour)
	{ 
		_colour = colour; 
		if (!_hold && !_hover) 
			_panel.SetColour(colour); 
		return *this;
	}
	UIButton& SetColourHold(const UIColour&colour)
	{ 
		_colourHold = colour; 
		if (_hold) 
			_panel.SetColour(colour);
		return *this;
	}
	UIButton& SetColourHover(const UIColour&colour)
	{
		_colourHover = colour;
		if (_hover && !_hold)
			_panel.SetColour(colour);
		return *this;
	}
	UIButton& SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }

	virtual void OnMouseMove(float mouseX, float mouseY) override;
	virtual void OnMouseUp() override;
	virtual void OnMouseDown() override;

	virtual void Render() const override
	{
		_panel.Render();
	}
};
