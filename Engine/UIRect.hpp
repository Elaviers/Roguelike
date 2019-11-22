#pragma once
#include "UIElement.hpp"
#include "Colour.hpp"
#include "GLTexture.hpp"
#include "Transform.hpp"
#include "Vector.hpp"

class UIRect : public UIElement
{
protected:
	const GLTexture *_texture;
	Colour _colour;

	Transform _transform;

public:
	UIRect(UIElement *parent = nullptr) : UIElement(parent), _texture(nullptr), _colour(1.f, 1.f, 1.f, 1.f) {}
	virtual ~UIRect() {}

	void SetTexture(const GLTexture *texture) { _texture = texture; }
	void SetColour(const Colour &colour) { _colour = colour; }

	virtual void Render() const override;

	virtual void OnBoundsChanged() override;
};
