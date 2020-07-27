#include "AnimationTrack.hpp"
#include <ELMaths/Quaternion.hpp>

bool AnimationTrack<Vector3>::_TypesAreAlmostEqual(const Vector3& a, const Vector3& b)
{
	return a.AlmostEquals(b, 0.001f);
}

bool AnimationTrack<Quaternion>::_TypesAreAlmostEqual(const Quaternion& a, const Quaternion& b)
{
	return a.GetData().AlmostEquals(b.GetData(), 0.001f);
}
