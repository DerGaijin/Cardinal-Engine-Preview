#pragma once
#include "../Definitions.h"
#include "Vec2.h"
#include "Vec3.h"

namespace CE
{
	struct Line2D 
	{
		Vec2f Start;
		Vec2f End;
	};

	struct Line3D 
	{
		Vec3f Start;
		Vec3f End;
	};
}
