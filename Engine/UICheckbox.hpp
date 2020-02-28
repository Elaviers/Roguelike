#pragma once
#include "UIButton.hpp"
#include "UIRect.hpp"

class UICheckbox : public UIElement
{
	bool _state;

	bool _readOnly;

	UIButton _button;
	UIRect _image;

	SharedPointer<const Texture> _textureFalse;
	SharedPointer<const Texture> _textureTrue;

	UIColour _colourFalse;
	UIColour _colourTrue;

	virtual void _OnBoundsChanged() override
	{
		_button.UpdateAbsoluteBounds();
		_image.UpdateAbsoluteBounds();
	}

	void _OnButtonPressed(UIButton&) { Toggle(); };

public:
	Event<UICheckbox&> onStateChanged;

	UICheckbox(UIElement* parent = nullptr) : UIElement(parent), _button(this), _image(this), _state(false), _readOnly(false)
	{
		_button.onPressed += FunctionPointer<void, UIButton&>(this, &UICheckbox::_OnButtonPressed);
	}

	virtual ~UICheckbox() {}

	bool GetState() const { return _state; }
	bool GetReadOnly() const { return _readOnly; }
	const SharedPointer<const Texture>& GetTextureFalse() const { return _textureFalse; }
	const SharedPointer<const Texture>& GetTextureTrue() const { return _textureTrue; }
	const UIColour& GetColourFalse() const { return _colourFalse; }
	const UIColour& GetColourTrue() const { return _colourTrue; }
	const UIColour& GetColourHover() const { return _button.GetColourHover(); }
	const UIColour& GetColourHold() const { return _button.GetColourHold(); }

	float GetBorderSize() const { return _button.GetBorderSize(); }
	const SharedPointer<const Material>& GetMaterial() const { return _button.GetMaterial(); }

	UICheckbox& SetState(bool state);
	UICheckbox& SetReadOnly(bool readOnly) { _readOnly = readOnly; return *this; }
	UICheckbox& SetTextureFalse(const SharedPointer<const Texture>& texture)
	{ 
		_textureFalse = texture;
		if (!_state) _image.SetTexture(texture);
		return *this;
	}
	UICheckbox& SetTextureTrue(const SharedPointer<const Texture>& texture)
	{ 
		_textureTrue = texture; 
		if (_state) _image.SetTexture(texture);
		return *this;
	}
	UICheckbox& SetColourFalse(const UIColour& colour) 
	{ 
		_colourFalse = colour; 
		if (!_state) _button.SetColour(colour);
		return *this; 
	}
	UICheckbox& SetColourTrue(const UIColour& colour) 
	{ 
		_colourTrue = colour; 
		if (_state) _button.SetColour(colour);
		return *this;
	}
	UICheckbox& SetColourHover(const UIColour& colour) { _button.SetColourHover(colour); return *this; }
	UICheckbox& SetColourHold(const UIColour& colour) { _button.SetColourHold(colour); return *this; }
	UICheckbox& SetMaterial(const SharedPointer<const Material>& material) { _button.SetMaterial(material); return *this; }
	UICheckbox& SetBorderSize(float borderSize) { _button.SetBorderSize(borderSize); return *this; }

	virtual void Render() const override
	{
		_button.Render();

		if ((_state && _textureTrue) || (!_state && _textureFalse))
			_image.Render();
	}

	virtual void OnMouseMove(float x, float y) { if (!_readOnly) _button.OnMouseMove(x, y); }
	virtual void OnMouseUp() { if (!_readOnly) _button.OnMouseUp(); }
	virtual void OnMouseDown() { if (!_readOnly) _button.OnMouseDown(); }

	void Toggle()
	{
		SetState(!_state);
	}
};
