#include "UICheckbox.hpp"

UICheckbox& UICheckbox::SetState(bool state)
{
	if (_state != state)
	{
		_state = state;
		_image.SetTexture(_state ? _textureTrue : _textureFalse);
		_button.SetColour(_state ? _colourTrue : _colourFalse);
		onStateChanged(*this);
	}

	return *this;
}
