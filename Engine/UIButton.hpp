#pragma once
#include "UIElement.hpp"
#include "UILabel.hpp"
#include "UIPanel.hpp"
#include "Colour.hpp"
#include "FunctionPointer.hpp"

class UIButton : public UIElement
{
protected:
	bool _active;

	FunctionPointer<void, UIButton&> _callback;

	Colour _colourInactive;
	Colour _colourActive;

	UIPanel _panel;
	UILabel _label;

public:
	UIButton(UIElement *parent = nullptr) : UIElement(parent), _active(false), _panel(this), _label(this) { }

	virtual ~UIButton() {}

	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const Colour& GetColourInactive() const { return _colourInactive; }
	const Colour& GetColourActive() const { return _colourActive; }
	const SharedPointer<const Font>& GetFont() const { return _label.GetFont(); }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }
	const String& GetString() const { return _label.GetString(); }

	void SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); }
	void SetCallback(const FunctionPointer<void, UIButton&> &callback) { _callback = callback; }
	void SetColourInactive(const Colour &colour) 
	{ 
		_colourInactive = colour; 
		if (!_active) 
			_panel.SetColour(colour); 
	}
	void SetColourActive(const Colour &colour)
	{ 
		_colourActive = colour; 
		if (_active) 
			_panel.SetColour(colour); 
	}
	void SetFont(const SharedPointer<const Font>& font) { _label.SetFont(font); }
	void SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); }
	void SetString(const String &string) { _label.SetString(string); }

	virtual void OnMouseMove(float mouseX, float mouseY) override;
	virtual void OnClick() override;

	virtual void Render() const override
	{
		_panel.Render();
		_label.Render();
	}

	virtual void OnBoundsChanged() override
	{
		_panel.UpdateAbsoluteBounds();
		_label.UpdateAbsoluteBounds();
	}
};
