#pragma once
#include "../Definitions.h"
#include "Vec3.h"
#include "Mat4x4.h"

namespace CE
{
	class CE_API Transform
	{
	public:
		// Constructor
		Transform() = default;

		// Constructor with Location
		Transform(const Vec3f& Location) : Location(Location) {}

		// Constructor with Location and Rotation
		Transform(const Vec3f& Location, const Vec3f& Rotation) : Location(Location), Rotation(Rotation) {}

		// Constructor with Location, Rotation and Scale
		Transform(const Vec3f& Location, const Vec3f& Rotation, const Vec3f& Scale) : Location(Location), Rotation(Rotation), Scale(Scale) {}

		// Returns the Matrix of the Transform
		Mat4x4f ToMatrix() const;

	public:
		//@TODO: I have no Idea how to explain this...?
		Transform& operator*=(const Transform& Other);

		//@TODO: I have no Idea how to explain this...?
		Transform operator*(const Transform& Other) const;

		bool operator==(const Transform& Other) const;
		
		bool operator!=(const Transform& Other) const;

	public:
		Vec3f Location = Vec3f(0.0f);
		Vec3f Rotation = Vec3f(0.0f);
		Vec3f Scale = Vec3f(1.0f);
	};
}
