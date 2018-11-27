#include "UIContainer.h"

void UIContainer::_OnParented(UIElement *child)
{
	_children.Add(child);
}

void UIContainer::_OnUnparented(UIElement *child)
{
	_children.Remove(child);
}

void UIContainer::Render()
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->Render();
}

void UIContainer::OnBoundsChanged()
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->UpdateAbsoluteBounds();
}

void UIContainer::OnMouseMove(float mouseX, float mouseY)
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->OnMouseMove(mouseX, mouseY);
}

void UIContainer::OnClick()
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->OnClick();
}
