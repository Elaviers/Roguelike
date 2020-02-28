#pragma once
#include "UIRect.hpp"
#include "List.hpp"

class UISplitter : public UIRect
{
	bool _hover;
	bool _drag;

	bool _isHorizontal;

	List<UIElement*> _after;

	float _textureTileSize;

	virtual void _OnBoundsChanged() override;

public:
	Event<UISplitter&> onDragged;

	UISplitter(UIElement* parent = nullptr) : UIRect(parent), _hover(false), _drag(false), _isHorizontal(false), _textureTileSize(32.f) { SetCursor(ECursor::ARROWS_HORIZONTAL); }
	virtual ~UISplitter() {}

	bool GetIsHorizontal() const { return _isHorizontal; }
	float GetTextureTileSize() const { return _textureTileSize; }
	
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

	virtual void Render() const override;

	virtual void OnMouseMove(float x, float y) override;
	virtual void OnMouseUp() override;
	virtual void OnMouseDown() override;
}; 
