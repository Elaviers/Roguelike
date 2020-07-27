#pragma once
#include "Rect.hpp"
#include <ELCore/List.hpp>

class UISplitter : public UIRect
{
	bool _drag;

	bool _isHorizontal;

	List<UIElement*> _after;

	float _textureTileSize;

	float _min;
	float _max;
	bool _useAbsolute;

	virtual void _OnBoundsChanged() override;

public:
	Event<UISplitter&> onDragged;

	UISplitter(UIElement* parent = nullptr) : UIRect(parent), 
		_drag(false), _isHorizontal(false), _textureTileSize(32.f), _min(0.1f), _max(0.9f), _useAbsolute(false) 
	{ SetCursor(ECursor::ARROWS_HORIZONTAL); }
	virtual ~UISplitter() {}

	bool GetIsHorizontal() const { return _isHorizontal; }
	float GetTextureTileSize() const { return _textureTileSize; }
	float GetMin() const { return _min; }
	float GetMax() const { return _max; }
	bool GetUseAbsolute() const { return _useAbsolute; }

	UISplitter& SetIsHorizontal(bool isHorizontal) 
	{ 
		_isHorizontal = isHorizontal; 
		SetCursor(_isHorizontal ? ECursor::ARROWS_VERTICAL : ECursor::ARROWS_HORIZONTAL);
		_transform.SetRotationEuler(_isHorizontal ? Vector3() : Vector3(0.f, 0.f, 90.f));
		return *this; 
	}
	UISplitter& SetTextureTileSize(float textureTileSize) { _textureTileSize = textureTileSize; return *this; }

	UISplitter& ShowSiblingBefore(UIElement* sibling)
	{
		_after.Remove(sibling);
		return *this;
	}

	UISplitter& ShowSiblingAfter(UIElement* sibling) 
	{
		if (!_after.Contains(sibling))
			_after.Add(sibling);

		return *this;
	}
	UISplitter& SetMin(float min) { _min = min; return *this; }
	UISplitter& SetMax(float max) { _max = max; return *this; }
	UISplitter& SetUseAbsolute(bool useAbsolute) { _useAbsolute = useAbsolute; return *this; }

	virtual void Render(RenderQueue&) const override;

	virtual bool OnMouseUp() override;
	virtual bool OnMouseDown() override;

	virtual void OnMouseMove(float x, float y) override;
}; 
