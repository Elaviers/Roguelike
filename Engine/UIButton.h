#pragma once
#include "UIElement.h"
#include "UILabel.h"
#include "UIPanel.h"
#include "Callback.h"

class UIButton : public UIElement
{
protected:
	bool _active;

	Callback _callback;

	Vector4 _colourInactive;
	Vector4 _colourActive;

	UIPanel _panel;
	UILabel _label;

public:
	UIButton(UIElement *parent = nullptr) : UIElement(parent), _panel(this), _label(this) { }

	virtual ~UIButton() {}

	inline void SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); }
	inline void SetCallback(const Callback &callback) { _callback = callback; }
	inline void SetColourInctive(const Vector4 &colour) 
	{ 
		_colourInactive = colour; 
		if (!_active) 
			_panel.SetColour(colour); 
	}
	inline void SetColourActive(const Vector4 &colour) 
	{ 
		_colourActive = colour; 
		if (_active) 
			_panel.SetColour(colour); 
	}
	inline void SetMaterial(const Material *material) { _panel.SetMaterial(material); }
	inline void SetString(const String &string) { _label.SetString(string); }
	inline void SetFont(const Font *font) { _label.SetFont(font); }

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
