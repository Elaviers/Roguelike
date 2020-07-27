#include "Checkbox.hpp"

UICheckbox& UICheckbox::SetState(bool state)
{
	if (_value != state)
	{
		_value = state;
		_image.SetTexture(_value ? _textureTrue : _textureFalse);
		_button.SetColour(_value ? _colourTrue : _colourFalse);
		onStateChanged(*this);
	}

	return *this;
}
