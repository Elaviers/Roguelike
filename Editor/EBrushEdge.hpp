#pragma once

enum class E2DBrushEdge
{
	NEG_X = 0, POS_X = 1, NEG_Y = 2, POS_Y = 3
};

enum class E3DBrushEdge
{
	NEG_X = (int)E2DBrushEdge::NEG_X,
	POS_X = (int)E2DBrushEdge::POS_X,
	NEG_Y = (int)E2DBrushEdge::NEG_Y,
	POS_Y = (int)E2DBrushEdge::POS_Y,
	NEG_Z = 4,
	POS_Z = 5
};
