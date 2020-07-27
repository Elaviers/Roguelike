#include "Container.hpp"

#define PASSTOEACHCHILD(FUNC_CALL) for (size_t i = 0; i < _children.GetSize(); ++i) _children[i]->FUNC_CALL
#define PASSTOEACHCHILD_RETURNIFTRUE(FUNC_CALL) for (size_t ti = _children.GetSize(); ti > 0; --ti) if (_children[ti - 1]->FUNC_CALL) return true;

void UIContainer::_OnChildGained(UIElement *child)
{
	_children.Add(child);
}

void UIContainer::_OnChildLost(UIElement *child)
{
	_children.Remove(child);
}

void UIContainer::Update(float deltaTime)
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->IsMarkedForDelete())
		{
			UIElement *element = _children[i];
			element->SetParent(nullptr); //Also removes from children
			delete element;
		}
		else _children[i]->Update(deltaTime);
}

void UIContainer::Clear() 
{ 
	while (_children.GetSize() > 0) 
		_children[0]->SetParent(nullptr); 
}

void UIContainer::FocusElement(UIElement* element)			{ PASSTOEACHCHILD(FocusElement(element)); }
void UIContainer::Render(RenderQueue& q) const				{ PASSTOEACHCHILD(Render(q)); }
void UIContainer::_OnBoundsChanged()						{ PASSTOEACHCHILD(UpdateAbsoluteBounds()); }

//TODO: Z-order is NOT taken into account for all of these functions, only render order!!! Kind of dumb!!!
bool UIContainer::OnKeyUp(EKeycode key)						{ PASSTOEACHCHILD_RETURNIFTRUE(OnKeyUp(key));	return false; }
bool UIContainer::OnKeyDown(EKeycode key)					{ PASSTOEACHCHILD_RETURNIFTRUE(OnKeyDown(key));	return false; }
bool UIContainer::OnCharInput(char c)						{ PASSTOEACHCHILD_RETURNIFTRUE(OnCharInput(c)); return false; }
bool UIContainer::OnMouseDown()								{ PASSTOEACHCHILD_RETURNIFTRUE(OnMouseDown());  return false; }
bool UIContainer::OnMouseUp()								{ PASSTOEACHCHILD_RETURNIFTRUE(OnMouseUp());  return false; }

//todo: ditto
void UIContainer::OnMouseMove(float mouseX, float mouseY) 
{ 
	_hover = false;

	_cursor = ECursor::DEFAULT;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		_children[i]->OnMouseMove(mouseX, mouseY);

		if (_children[i]->GetHover())
		{
			_hover = true;

			if ((_cursor == ECursor::ARROWS_HORIZONTAL && _children[i]->GetCursor() == ECursor::ARROWS_VERTICAL) ||
				(_cursor == ECursor::ARROWS_VERTICAL && _children[i]->GetCursor() == ECursor::ARROWS_HORIZONTAL))
				_cursor = ECursor::ARROWS_QUAD;
			else
				_cursor = _children[i]->GetCursor();
		}
	}

	_cursor = _cursor;
}
