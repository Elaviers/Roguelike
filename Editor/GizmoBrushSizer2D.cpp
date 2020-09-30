#include "GizmoBrushSizer2D.hpp"
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>

void GizmoBrushSizer2D::SetObjectTransform(const Transform& objectTransform)
{
	Vector3 pOff;

	float scale;
	switch (_edge)
	{
	case E2DBrushEdge::NEG_X:
		_normal = -objectTransform.GetRightVector();
		scale = objectTransform.GetScale()[0];
		pOff = objectTransform.GetForwardVector() * (objectTransform.GetScale()[2] / 2.f);
		break;
	case E2DBrushEdge::POS_X:
		_normal = objectTransform.GetRightVector();
		scale = objectTransform.GetScale()[0];
		pOff = objectTransform.GetForwardVector() * (objectTransform.GetScale()[2] / 2.f);
		break;
	case E2DBrushEdge::NEG_Y:
		_normal = -objectTransform.GetForwardVector();
		scale = objectTransform.GetScale()[2];
		pOff = objectTransform.GetRightVector() * (objectTransform.GetScale()[0] / 2.f);
		break;
	case E2DBrushEdge::POS_Y:
		_normal = objectTransform.GetForwardVector();
		scale = objectTransform.GetScale()[2];
		pOff = objectTransform.GetRightVector() * (objectTransform.GetScale()[0] / 2.f);
		break;

	default:
		Debug::Break();
		return;
	}

	_edgePoint = objectTransform.GetPosition() + _normal * (scale / 2.f);
	_lineA = _edgePoint + pOff;
	_lineB = _edgePoint - pOff;
}

void GizmoBrushSizer2D::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);

	e.AddSetLineWidth(2.f);
	e.AddLine(_lineA, _lineB);
}

void GizmoBrushSizer2D::Update(const MouseData& mouseData, const Ray& mouseRay, float& maxT)
{
	Vector3 line2camera = (mouseRay.origin - _lineA).Normalise();
	Vector3 ab = (_lineB - _lineA).Normalise();
	float t = Collision::IntersectRayPlane(mouseRay, _lineA, Vector3::TripleCross(ab, line2camera, ab));
	Vector3 pp = mouseRay.origin + mouseRay.direction * t;

	if (_isDragging)
	{
		_owner->GetSideFunction()(_edge, _normal * _normal.Dot(pp - _edgePoint));
	}
	else
	{
		if (t < maxT)
		{
			Vector3 cp = Collision::ClosestPointOnLineSegment(_lineA, _lineB, pp);
			float dist2 = (cp - pp).LengthSquared();

			if (dist2 <= 0.33f * 0.33f)
			{
				maxT = t;

				_canDrag = true;
				return;
			}
		}

		_canDrag = false;
	}
}
