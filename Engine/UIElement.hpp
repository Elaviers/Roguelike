#pragma once

class UIElement
{
protected:
	UIElement *_parent;

	bool _markedForDelete;

	//Absolute bounds
	float _x, _y, _w, _h;
	
	//Relative bounds
	struct {
		float x, y, w, h;
		float xOffset, yOffset;
	} _bounds;

	float _z;

protected:
	UIElement(UIElement *parent) : _parent(parent), _bounds{ 0.f, 0.f, 1.f, 1.f } 
	{ 
		if (_parent)
		{
			UpdateAbsoluteBounds();
			_parent->_OnParented(this);
		}
	}

	virtual void _OnParented(UIElement *child) {}
	virtual void _OnUnparented(UIElement *child) {}

public:
	virtual ~UIElement() 
	{
		if (_parent)
			_parent->_OnUnparented(this);
	}

	void UpdateAbsoluteBounds();

	inline void SetParent(UIElement *parent) 
	{ 
		if (_parent)
			_parent->_OnUnparented(this);

		_parent = parent;
		UpdateAbsoluteBounds();

		if (_parent)
			_parent->_OnParented(this);
	}

	inline void SetBounds(float x, float y, float w, float h, float xOffset = 0.f, float yOffset = 0.f)
	{
		_bounds.x = x;
		_bounds.y = y;
		_bounds.w = w;
		_bounds.h = h;
		_bounds.xOffset = xOffset;
		_bounds.yOffset = yOffset;
		UpdateAbsoluteBounds();
	}

	inline void SetZ(float z) { _z = z; UpdateAbsoluteBounds(); }

	inline void MarkForDelete() { _markedForDelete = true; }
	inline bool ShouldDelete() const { return _markedForDelete; }

	virtual void Render() const {}
	virtual void Update() {}

	virtual void OnBoundsChanged() {}
	virtual void OnMouseMove(float mouseX, float mouseY) {}
	virtual void OnClick() {}
};
