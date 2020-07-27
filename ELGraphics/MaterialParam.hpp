#pragma once

struct MaterialParam
{
	enum class EType
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
