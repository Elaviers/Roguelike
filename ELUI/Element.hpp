#pragma once
#include <ELCore/Event.hpp>
#include <ELSys/ECursor.hpp>
#include <ELSys/EKeycode.hpp>

class RenderQueue;

struct UICoord
{
	float relative;
	float absolute;

	UICoord(float relative = 0.f, float absolute = 0.f) : relative(relative), absolute(absolute) {}
};

struct UIBounds
{
	UICoord x;
	UICoord y;
	UICoord w;
	UICoord h;

	UIBounds() : x(0.f), y(0.f), w(1.f), h(1.f) {}
	UIBounds(const UICoord& x, const UICoord& y, const UICoord& w, const UICoord& h) : x(x), y(y), w(w), h(h) {}
};

struct AbsoluteBounds
{
	float x, y, w, h;

	AbsoluteBounds() : x(0.f), y(0.f), w(0.f), h(0.f) {}
};

class UIElement
{
	void _SetFocus(UIElement& element) { _hasFocus = true; }
	void _ClearFocus(UIElement& element) { _hasFocus = false; }

protected:
	UIElement *_parent;

	bool _markedForDelete;
	bool _hasFocus;
	bool _hover;

	bool _focusOnClick;

	UIBounds _bounds;
	AbsoluteBounds _absoluteBounds;

	float _z;

	ECursor _cursor;

public:
	Event<UIElement&> onFocusGained;
	Event<UIElement&> onFocusLost;

protected:
	UIElement(UIElement *parent) : _parent(parent), _markedForDelete(false), _hasFocus(false), _focusOnClick(true), _hover(false), _z(0.f), _cursor(ECursor::DEFAULT)
	{ 
		onFocusGained += FunctionPointer<void, UIElement&>(this, &UIElement::_SetFocus);
		onFocusLost += FunctionPointer<void, UIElement&>(this, &UIElement::_ClearFocus);

		if (_parent)
		{
			UpdateAbsoluteBounds();
			_parent->_OnChildGained(this);
		}
	}

	UIElement(const UIElement& other) = delete;

	virtual void _OnBoundsChanged() {}
	
	virtual void _OnChildGained(UIElement *child) {}
	virtual void _OnChildLost(UIElement *child) {}

public:
	virtual ~UIElement() 
	{
		if (_parent)
			_parent->_OnChildLost(this);
	}

	void UpdateAbsoluteBounds();

	UIElement& SetParent(UIElement *parent) 
	{ 
		if (_parent)
			_parent->_OnChildLost(this);

		_parent = parent;
		UpdateAbsoluteBounds();

		if (_parent)
			_parent->_OnChildGained(this);

		return *this;
	}

	UIElement& SetBounds(const UIBounds& bounds) { _bounds = bounds; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetBounds(const UICoord& x, const UICoord& y, const UICoord& w, const UICoord& h)
	{
		_bounds.x = x;
		_bounds.y = y;
		_bounds.w = w;
		_bounds.h = h;
		UpdateAbsoluteBounds();
		return *this;
	}
	UIElement& SetX(const UICoord& x) { _bounds.x = x; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetY(const UICoord& y) { _bounds.y = y; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetW(const UICoord& w) { _bounds.w = w; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetH(const UICoord& h) { _bounds.h = h; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetZ(float z) { _z = z; UpdateAbsoluteBounds(); return *this; }
	UIElement& SetCursor(const ECursor& cursor) { _cursor = cursor; return *this; }
	UIElement& SetFocusOnClick(bool focusOnClick) { _focusOnClick = focusOnClick; return *this; }
	void MarkForDelete() { _markedForDelete = true; }

	const AbsoluteBounds& GetAbsoluteBounds() const { return _absoluteBounds; }
	const UIBounds& GetBounds() const { return _bounds; }
	float GetZ() const { return _z; }
	const ECursor& GetCursor() const { return _cursor; }
	bool IsMarkedForDelete() const { return _markedForDelete; }
	bool HasFocus() const { return _hasFocus; }
	bool GetFocusOnClick() const { return _focusOnClick; }

	//True if mouse is currently hovering over the element
	bool GetHover() const { return _hover; }

	void RequestFocus();

	virtual bool OverlapsPoint(float x, float y) const
	{
		return x >= _absoluteBounds.x &&
			x <= (_absoluteBounds.x + _absoluteBounds.w) &&
			y >= _absoluteBounds.y &&
			y <= (_absoluteBounds.y + _absoluteBounds.h);
	}

	virtual void FocusElement(UIElement* child);

	virtual void Render(RenderQueue&) const {}
	virtual void Update(float deltaTime) {}

	//These functions return true if they 'consume' the input
	virtual bool OnKeyUp(EKeycode) { return false; }
	virtual bool OnKeyDown(EKeycode) { return false; }
	virtual bool OnCharInput(char) { return false; }
	virtual bool OnMouseUp();
	virtual bool OnMouseDown();

	virtual void OnMouseMove(float mouseX, float mouseY);

	virtual void OnHoverStart() {}
	virtual void OnHoverStop() {}
};
