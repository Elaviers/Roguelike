#include "ListBox.hpp"

void UIListBox::_UpdateLabels()
{
	size_t i = 1;
	for (UILabel& label : _labels)
	{
		label.SetBounds(0.f, UICoord(1.f, i * -_itemHeight), 1.f, UICoord(0.f, _itemHeight));
		label.SetFont(_font);
		label.SetColour(_textColour);
		label.SetShadowColour(_textShadowColour);
		label.SetShadowOffset(_textShadowOffset);
		++i;
	}
}

void UIListBox::Add(const String& item)
{
	size_t index = _labels.GetSize() + 1;

	UILabel& newLabel = *_labels.Emplace(this);
	newLabel.SetBounds(0.f, UICoord(1.f, index * -_itemHeight), 1.f, UICoord(0.f, _itemHeight));
	newLabel.SetFont(_font);
	newLabel.SetColour(_textColour);
	newLabel.SetShadowColour(_textShadowColour);
	newLabel.SetShadowOffset(_textShadowOffset);
	newLabel.SetString(item);
}

void UIListBox::OnMouseMove(float x, float y)
{
	UIElement::OnMouseMove(x, y);

	if (_hover)
	{
		for (UILabel& label : _labels)
		{
			if (label.OverlapsPoint(x, y))
			{
				if (&label != _selectedLabel)
				{
					_selectedLabel = &label;
					_selectionBox.SetBounds(_selectedLabel->GetBounds());
					onSelectionChanged(*_selectedLabel);
				}

				break;
			}
		}
	}
}

bool UIListBox::OnMouseUp()
{
	if (_hover && _selectedLabel)
	{
		onSelectionSubmitted(*_selectedLabel);
	}

	return _hover;
}
