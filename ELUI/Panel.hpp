#pragma once
#include "Element.hpp"
#include "Colour.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELMaths/Transform.hpp>
#include <ELGraphics/Material.hpp>

class UIPanel : public UIElement
{
	SharedPointer<const Material> _material;
	UIColour _colour;

	float _borderSize;

	Transform _transforms[9];

public:
	UIPanel(UIElement *parent = nullptr) : UIElement(parent), _material(nullptr), _colour(Colour::White), _borderSize(0.f) {}
	virtual ~UIPanel() {}

	virtual void Render(RenderQueue&) const override;

	virtual void _OnBoundsChanged() override;

	float GetBorderSize() const { return _borderSize; }
	const SharedPointer<const Material>& GetMaterial() const { return _material;}
	const UIColour& GetColour() const { return _colour; }

	UIPanel& SetBorderSize(float size) { _borderSize = size; _OnBoundsChanged(); return *this; }
	UIPanel& SetColour(const UIColour& colour) { _colour = colour; return *this; }
	UIPanel& SetMaterial(const SharedPointer<const Material>& material) { _material = material; return *this; }
};
