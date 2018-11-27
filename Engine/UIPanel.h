#pragma once
#include "UIElement.h"
#include "Material.h"
#include "Transform.h"

class UIPanel : public UIElement
{
	const Material *_material;
	Vector4 _colour;

	float _borderSize;

	Transform _transforms[9];

public:
	UIPanel(UIElement *parent = nullptr) : UIElement(parent) {}
	virtual ~UIPanel() {}

	virtual void Render() const override;

	virtual void OnBoundsChanged() override;

	inline void SetBorderSize(float size) { _borderSize = size; OnBoundsChanged(); }
	inline void SetColour(const Vector4 &colour) { _colour = colour; }
	inline void SetMaterial(const Material *material) { _material = material; }
};
