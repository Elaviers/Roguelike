#pragma once

//Used for rendering materials

struct RenderParam
{
	enum class Type
	{
		GRID_PARAM //Parameters for a MaterialGrid
	} type;

	union
	{
		struct
		{
			int row;
			int column;
		} gridData;

	};
};
