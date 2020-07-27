#include "Textbox.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void UITextbox::_OnBoundsChanged()
{
	_label.UpdateAbsoluteBounds();
	_panel.UpdateAbsoluteBounds();
	_selectionRect.UpdateAbsoluteBounds();
}

void UITextbox::_ResetCaretBlink()
{
	_caretTimer = _caretPeriod;
	_caretStatus = true;
}

void UITextbox::_UpdateSelectionBox()
{
	Transform t = _label.GetRenderTransform();

	float startX = t.GetPosition().x + (_selectionStart == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetString().GetData(), _label.GetRenderTransform().GetScale().x, _selectionStart));
	float endX = t.GetPosition().x + (_selectionEnd == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetString().GetData(), _label.GetRenderTransform().GetScale().x, _selectionEnd));

	if (startX > endX) Utilities::Swap(startX, endX);

	_selectionRect.SetBounds(UICoord(0.f, startX - _absoluteBounds.x), 0.f, UICoord(0.f, endX - startX), 1.f);
}

void UITextbox::_RemoveSelection()
{
	if (_selectionStart > _selectionEnd) Utilities::Swap(_selectionStart, _selectionEnd);

	SetString(GetString().SubString(0, _selectionStart) + GetString().SubString(_selectionEnd));

	_caretPos = _selectionStart;
	_selectionStart = _selectionEnd = 0;
	_UpdateSelectionBox();
}

void UITextbox::Render(RenderQueue& q) const
{
	_panel.Render(q);
	_label.Render(q);
	_selectionRect.Render(q);

	if (_hasFocus && _caretStatus)
	{
		Transform t = _label.GetRenderTransform();
		float caretX = _caretOffset * t.GetScale().x + (_caretPos == 0 ? 0.f : _label.GetFont()->CalculateStringWidth(_label.GetString().GetData(), t.GetScale().x, _caretPos));
		t.Move(Vector3(caretX, _absoluteBounds.h / 2.f, 0.f));
		t.SetScale(Vector3(_caretWidth, t.GetScale().y, 0.f));

		//todo- priority
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT, 0);
		_caretColour.Apply(e);

		e.AddSetTransform(t.GetTransformationMatrix());
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void UITextbox::Update(float deltaTime)
{
	_caretTimer -= deltaTime;

	if (_caretTimer <= 0.f)
	{
		_caretTimer = _caretPeriod;
		_caretStatus = !_caretStatus;
	}
}

bool UITextbox::OnKeyDown(EKeycode key)
{
	if (_hasFocus && !_readOnly)
	{
		switch (key)
		{
		case EKeycode::LEFT:
			if (_caretPos > 0) --_caretPos;

			_ResetCaretBlink();
			break;
		
		case EKeycode::RIGHT:
			if (_caretPos < GetString().GetLength()) ++_caretPos;

			_ResetCaretBlink();
			break;

		case EKeycode::ENTER:
			onStringSubmitted(*this);
			break;
		}

		return true;
	}

	return false;
}

bool UITextbox::OnKeyUp(EKeycode kc)
{
	return _hasFocus && !_readOnly;
}

bool UITextbox::OnCharInput(char c)
{
	if (_hasFocus && !_readOnly)
	{
		if (c == '\b')
		{
			if (_selectionStart != _selectionEnd)
			{
				_RemoveSelection();
			}
			else if (_caretPos > 0)
			{
				SetString(GetString().SubString(0, _caretPos - 1) + GetString().SubString(_caretPos));
				--_caretPos;

				_ResetCaretBlink();
			}

			onStringChanged(*this);
		}
		else if (c >= ' ')
		{
			if (_selectionStart != _selectionEnd)
				_RemoveSelection();

			SetString(GetString().SubString(0, _caretPos) + c + GetString().SubString(_caretPos));
			++_caretPos;

			_ResetCaretBlink();

			onStringChanged(*this);
		}

		return true;
	}

	return false;
}

void UITextbox::OnMouseMove(float x, float y)
{
	UIElement::OnMouseMove(x, y);

	if (_hover)
	{
		_lastMouseX = x;
		_lastMouseY = y;
	}

	if (_selecting)
	{
		_caretPos = _label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetString().GetData(), _label.GetRenderTransform());
		_selectionEnd = _caretPos;

		_UpdateSelectionBox();
	}
}

bool UITextbox::OnMouseUp()
{
	_selecting = false;

	return _hover;
}

bool UITextbox::OnMouseDown()
{
	if (_hover)
	{
		RequestFocus();
		
		_caretPos = _label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetString().GetData(), _label.GetRenderTransform());

		_selecting = true;
		_selectionStart = _caretPos;
		_selectionEnd = _caretPos;

		_ResetCaretBlink();
		_UpdateSelectionBox();

		return true;
	}

	return false;
}
