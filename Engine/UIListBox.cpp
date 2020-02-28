#include "UIListBox.hpp"

void UIListBox::_UpdateLabels()
{
	size_t i = 1;
	for (auto it = _labels.First(); it.IsValid(); ++it)
	{
		it->SetBounds(0.f, 1.f, 1.f, _itemHeight, 0.f, i * -_itemHeight);
		it->SetFont(_font);
		it->SetColour(_textColour);
		it->SetShadowColour(_textShadowColour);
		it->SetShadowOffset(_textShadowOffset);
		++i;
	}
}

void UIListBox::Add(const String& item)
{
	size_t index = _labels.GetSize() + 1;

	UILabel& newLabel = *_labels.Emplace(this);
	newLabel.SetBounds(0.f, 1.f, 1.f, _itemHeight, 0.f, index * -_itemHeight);
	newLabel.SetFont(_font);
	newLabel.SetColour(_textColour);
	newLabel.SetShadowColour(_textShadowColour);
	newLabel.SetShadowOffset(_textShadowOffset);
	newLabel.SetString(item);
}

void UIListBox::OnMouseMove(float x, float y)
{
	_hover = OverlapsPoint(x, y);

	if (_hover)
	{
		for (auto it = _labels.First(); it.IsValid(); ++it)
		{
			if (it->OverlapsPoint(x, y))
			{
				if (&*it != _selectedLabel)
				{
					_selectedLabel = &*it;
					_selectionBox.SetBounds(_selectedLabel->GetRelativeBounds());
					onSelectionChanged(*_selectedLabel);
				}

				break;
			}
		}
	}
}

void UIListBox::OnMouseUp()
{
	if (_hover && _selectedLabel)
	{
		onSelectionSubmitted(*_selectedLabel);
	}
}
