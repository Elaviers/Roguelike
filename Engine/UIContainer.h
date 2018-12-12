#pragma once
#include "UIElement.h"
#include "Buffer.h"

class UIContainer : public UIElement
{
protected:
	Buffer<UIElement*> _children;

	virtual void _OnParented(UIElement *child) override;
	virtual void _OnUnparented(UIElement *child) override;

public:
	UIContainer(UIElement *parent = nullptr) : UIElement(parent) {}
	virtual ~UIContainer() {}

	virtual void Render() const override;
	virtual void Update() override;
	
	virtual void OnBoundsChanged();
	virtual void OnMouseMove(float mouseX, float mouseY);
	virtual void OnClick();
};

