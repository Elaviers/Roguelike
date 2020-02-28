#include "UIContainer.hpp"

#define PASSTOEACHCHILD(FUNC_CALL) for (uint32 i = 0; i < _children.GetSize(); ++i) _children[i]->FUNC_CALL

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

void UIContainer::OnElementRequestFocus(UIElement* element) { PASSTOEACHCHILD(OnElementRequestFocus(element)); }
void UIContainer::Render() const { PASSTOEACHCHILD(Render()); }
void UIContainer::_OnBoundsChanged() { PASSTOEACHCHILD(UpdateAbsoluteBounds());}
void UIContainer::OnKeyUp(EKeycode key) { PASSTOEACHCHILD(OnKeyUp(key));}
void UIContainer::OnKeyDown(EKeycode key){ PASSTOEACHCHILD(OnKeyDown(key));}
void UIContainer::OnCharInput(char c) { PASSTOEACHCHILD(OnCharInput(c)); }
void UIContainer::OnMouseUp() { PASSTOEACHCHILD(OnMouseUp()); }
void UIContainer::OnMouseDown() { PASSTOEACHCHILD(OnMouseDown()); }

UIElement* UIContainer::GetLastOverlappingElement(float x, float y, ECursor& out_Cursor) const
{
	UIElement* last = nullptr;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		UIContainer* container = dynamic_cast<UIContainer*>(_children[i]);
		if (container)
		{
			ECursor cursor = ECursor::DEFAULT;
			UIElement* element = container->GetLastOverlappingElement(x, y, cursor);
			if (element)
			{
				last = element;
			
				if ((out_Cursor == ECursor::ARROWS_HORIZONTAL && cursor == ECursor::ARROWS_VERTICAL) ||
					(out_Cursor == ECursor::ARROWS_VERTICAL && cursor == ECursor::ARROWS_HORIZONTAL))
					out_Cursor = ECursor::ARROWS_QUAD;
				else
					out_Cursor = cursor;
			}
		}
		else if (_children[i]->OverlapsPoint(x, y))
		{
			last = _children[i];

			if ((out_Cursor == ECursor::ARROWS_HORIZONTAL && _children[i]->GetCursor() == ECursor::ARROWS_VERTICAL) ||
				(out_Cursor == ECursor::ARROWS_VERTICAL && _children[i]->GetCursor() == ECursor::ARROWS_HORIZONTAL))
				out_Cursor = ECursor::ARROWS_QUAD;
			else
				out_Cursor = _children[i]->GetCursor();
		}
	}

	return last;
}

void UIContainer::OnMouseMove(float mouseX, float mouseY) 
{ 
	PASSTOEACHCHILD(OnMouseMove(mouseX, mouseY)); 

	ECursor cursor = ECursor::DEFAULT;
	UIElement* element = GetLastOverlappingElement(mouseX, mouseY, cursor);
	if (element)
		Engine::UseCursor(cursor);
}
