#pragma once
#include "Vector3.hpp"

struct Frustum
{
	union
	{
		struct
		{
			Vector3 normalNear;
			Vector3 normalTop;
			Vector3 normalBottom;
			Vector3 normalLeft;
			Vector3 normalRight;
			Vector3 normalFar;
		};

		Vector3 normals[6];
	};

	Vector3 position;

	union
	{
		struct
		{
			Vector3 nearBtmLeft;	//near
			Vector3 nearTopLeft;	//top
			Vector3 nearBtmRight;	//btm
			Vector3 farBtmLeft;		//left
			Vector3 farBtmRight;	//right
			Vector3 farTopLeft;		//far
			Vector3 farTopRight;	//leftover
			Vector3 nearTopRight;	//leftover

			//These are defined in this order so the indices match with valid normals
		};

		Vector3 points[8];
	};

	bool OverlapsSphere(const Vector3& pos, float radius) const;
	bool OverlapsAABB(const Vector3& min, const Vector3& max) const;

	Frustum() {}
};
