#pragma once
#include "UIElement.h"
#include "UILabel.h"
#include "UIPanel.h"
#include "FunctionPointer.h"

class UIButton : public UIElement
{
protected:
	bool _active;

	FunctionPointer<void, UIButton&> _callback;

	Vector4 _colourInactive;
	Vector4 _colourActive;

	UIPanel _panel;
	UILabel _label;

public:
	UIButton(UIElement *parent = nullptr) : UIElement(parent), _panel(this), _label(this) { }

	virtual ~UIButton() {}

	inline float GetBorderSize() const { return _panel.GetBorderSize(); }
	inline const Vector4 &GetColourInactive() const { return _colourInactive; }
	inline const Vector4 &GetColourActive() const { return _colourActive; }
	inline const Font* GetFont() const { return _label.GetFont(); }
	inline const Material* GetMaterial() const { return _panel.GetMaterial(); }
	inline const String& GetString() const { return _label.GetString(); }

	inline void SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); }
	inline void SetCallback(const FunctionPointer<void, UIButton&> &callback) { _callback = callback; }
	inline void SetColourInactive(const Vector4 &colour) 
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
	inline void SetFont(const Font *font) { _label.SetFont(font); }
	inline void SetMaterial(const Material *material) { _panel.SetMaterial(material); }
	inline void SetString(const String &string) { _label.SetString(string); }

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
