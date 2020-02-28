#pragma once
#include "EKeycode.hpp"
#include "Engine.hpp"
#include "Event.hpp"

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
protected:
	UIElement *_parent;

	bool _markedForDelete;

	UIBounds _bounds;
	AbsoluteBounds _absoluteBounds;

	float _z;

	ECursor _cursor;

protected:
	UIElement(UIElement *parent) : _parent(parent), _markedForDelete(false), _z(0.f), _cursor(ECursor::DEFAULT)
	{ 
		if (_parent)
		{
			UpdateAbsoluteBounds();
			_parent->_OnChildGained(this);
		}
	}

	virtual void _OnBoundsChanged() {}
	
	virtual void _OnChildGained(UIElement *child) {}
	virtual void _OnChildLost(UIElement *child) {}

	void _RequestFocus();

public:
	Event<UIElement&> onFocusGained;
	Event<UIElement&> onFocusLost;

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

	const AbsoluteBounds& GetAbsoluteBounds() const { return _absoluteBounds; }
	const UIBounds& GetBounds() const { return _bounds; }
	float GetZ() const { return _z; }
	const ECursor& GetCursor() const { return _cursor; }

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

	void MarkForDelete() { _markedForDelete = true; }
	bool IsMarkedForDelete() const { return _markedForDelete; }

	virtual bool OverlapsPoint(float x, float y) const
	{
		return x >= _absoluteBounds.x &&
			x <= (_absoluteBounds.x + _absoluteBounds.w) &&
			y >= _absoluteBounds.y &&
			y <= (_absoluteBounds.y + _absoluteBounds.h);
	}

	virtual void OnElementRequestFocus(UIElement* child);

	virtual void Render() const {}
	virtual void Update(float deltaTime) {}

	virtual void OnKeyUp(EKeycode) {}
	virtual void OnKeyDown(EKeycode) {}
	virtual void OnCharInput(char) {}
	virtual void OnMouseMove(float mouseX, float mouseY) {}
	virtual void OnMouseUp() {}
	virtual void OnMouseDown() {}
};
