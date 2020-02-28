#pragma once
#include "UIElement.hpp"
#include "Buffer.hpp"

class UIContainer : public UIElement
{
protected:
	Buffer<UIElement*> _children;

	virtual void _OnBoundsChanged();

	virtual void _OnChildGained(UIElement *child) override;
	virtual void _OnChildLost(UIElement *child) override;

public:
	UIContainer(UIElement *parent = nullptr) : UIElement(parent) {}
	virtual ~UIContainer() {}

	const Buffer<UIElement*>& GetChildren() const { return _children; }

	void Clear();

	virtual void OnElementRequestFocus(UIElement*) override;

	virtual void Render() const override;
	virtual void Update(float deltaTime) override;
	
	virtual void OnKeyUp(EKeycode);
	virtual void OnKeyDown(EKeycode);
	virtual void OnCharInput(char);
	virtual void OnMouseMove(float mouseX, float mouseY);
	virtual void OnMouseUp();
	virtual void OnMouseDown();

	UIElement* GetLastOverlappingElement(float x, float y, ECursor& out_Cursor) const;
};
