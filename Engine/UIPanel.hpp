#pragma once
#include "UIElement.hpp"
#include "Colour.hpp"
#include "Material.hpp"
#include "Transform.hpp"

class UIPanel : public UIElement
{
	SharedPointer<const Material> _material;
	Colour _colour;

	float _borderSize;

	Transform _transforms[9];

public:
	UIPanel(UIElement *parent = nullptr) : UIElement(parent), _material(nullptr), _colour(1.f, 1.f, 1.f, 1.f), _borderSize(0.f) {}
	virtual ~UIPanel() {}

	virtual void Render() const override;

	virtual void OnBoundsChanged() override;

	float GetBorderSize() const { return _borderSize; }
	const SharedPointer<const Material>& GetMaterial() const { return _material;}
	const Colour& GetColour() const { return _colour; }

	void SetBorderSize(float size) { _borderSize = size; OnBoundsChanged(); }
	void SetColour(const Colour &colour) { _colour = colour; }
	void SetMaterial(const SharedPointer<const Material> &material) { _material = material; }
};
