#pragma once
#include "UIElement.hpp"
#include "UIColour.hpp"
#include "Material.hpp"
#include "Transform.hpp"

class UIPanel : public UIElement
{
	SharedPointer<const Material> _material;
	UIColour _colour;

	float _borderSize;

	Transform _transforms[9];

public:
	UIPanel(UIElement *parent = nullptr) : UIElement(parent), _material(nullptr), _colour(Colour::White), _borderSize(0.f) {}
	virtual ~UIPanel() {}

	virtual void Render() const override;

	virtual void _OnBoundsChanged() override;

	float GetBorderSize() const { return _borderSize; }
	const SharedPointer<const Material>& GetMaterial() const { return _material;}
	const UIColour& GetColour() const { return _colour; }

	UIPanel& SetBorderSize(float size) { _borderSize = size; _OnBoundsChanged(); return *this; }
	UIPanel& SetColour(const UIColour& colour) { _colour = colour; return *this; }
	UIPanel& SetMaterial(const SharedPointer<const Material>& material) { _material = material; return *this; }
};
