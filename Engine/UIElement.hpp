#pragma once
#include "EKeycode.hpp"
#include "Engine.hpp"
#include "Event.hpp"

struct RelativeBounds
{
	float x, y, w, h;
	float xOffset, yOffset;
	float wOffset, hOffset;
};

struct AbsoluteBounds
{
	float x, y, w, h;
};

class UIElement
{
protected:
	UIElement *_parent;

	bool _markedForDelete;

	RelativeBounds _relativeBounds;
	AbsoluteBounds _absoluteBounds;

	float _z;

	ECursor _cursor;

protected:
	UIElement(UIElement *parent) : _parent(parent), _relativeBounds{ 0.f, 0.f, 1.f, 1.f }, _cursor(ECursor::DEFAULT)
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
	const RelativeBounds& GetRelativeBounds() const { return _relativeBounds; }
	float GetZ() const { return _z; }
	const ECursor& GetCursor() const { return _cursor; }

	UIElement& SetBounds(const RelativeBounds& bounds) { _relativeBounds = bounds; UpdateAbsoluteBounds(); return *this; }

	UIElement& SetBounds(float x, float y, float w, float h, float xOffset = 0.f, float yOffset = 0.f, float wOffset = 0.f, float hOffset = 0.f)
	{
		_relativeBounds.x = x;
		_relativeBounds.y = y;
		_relativeBounds.w = w;
		_relativeBounds.h = h;
		_relativeBounds.xOffset = xOffset;
		_relativeBounds.yOffset = yOffset;
		_relativeBounds.wOffset = wOffset;
		_relativeBounds.hOffset = hOffset;
		UpdateAbsoluteBounds();
		return *this;
	}

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
