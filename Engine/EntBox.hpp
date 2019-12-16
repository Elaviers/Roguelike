#pragma once
#include "Entity.hpp"
#include "Colour.hpp"
#include "Types.hpp"
#include "Vector.hpp"

class EntBox : public Entity
{
	Vector3 _p1, _p2;

	void _UpdateMinMax();
	void _OnTransformChanged();

protected:
	Vector3 _min, _max;

	Colour _colour;

public:
	Entity_FUNCS(EntBox, EntityID::BOX)

	EntBox(Flags flags = Flags::NONE) : Entity(flags), _colour(1.f, 1.f, 1.f, 1.f) {}
	virtual ~EntBox() {}

	virtual void Render(RenderChannels) const override;

	virtual Bounds GetBounds() const override { return Bounds(_min, _max); }

	void SetPoint1(const Vector3 &point1) 
	{
		_p1 = point1;
		_UpdateMinMax();
	}

	void SetPoint2(const Vector3 &point2)
	{
		_p2 = point2;
		_UpdateMinMax();
	}

	void SetMin(const Vector3 &min) { _min = min; }
	void SetMax(const Vector3 &max) { _max = max; }

	void SetRenderColour(const Colour &colour) { _colour = colour; }

	const Vector3& GetPoint1() const { return _p1; }
	const Vector3& GetPoint2() const { return _p2; }
	const Vector3& GetMin() const { return _min; }
	const Vector3& GetMax() const { return _max; }
};
