#include "CardinalEngine/Framework/Core/Math/Transform.h"
#include "CardinalEngine/Framework/Core/Math/Quaternion.h"

namespace CE
{
	Mat4x4f Transform::ToMatrix() const
	{
		Mat4x4f Result = Mat4x4f::Identity();
		Result = Result.Translate(Location);
		Result *= Mat4x4f::FromQuaternion(Quaternion::FromEuler(Math::Radians(Rotation)));
		Result = Result.Scale(Scale);
		return Result;
	}

	Transform& Transform::operator*=(const Transform& Other)
	{
		Quaternion RotA = Quaternion::FromEuler(Math::Radians(Rotation));
		Quaternion RotB = Quaternion::FromEuler(Math::Radians(Other.Rotation));

		Rotation = Math::Degrees((RotB * RotA).ToEuler());

		Vec3f ScaledLocation = Location * Other.Scale;
		Vec3f RotatedLocation = RotB.Rotate(ScaledLocation);
		Location = RotatedLocation + Other.Location;

		Scale = Scale * Other.Scale;

		return *this;
	}

	Transform Transform::operator*(const Transform& Other) const
	{
		return Transform(*this) *= Other;
	}

	bool Transform::operator==(const Transform& Other) const
	{
		return Location == Other.Location && Rotation == Other.Rotation && Scale == Other.Scale;
	}

	bool Transform::operator!=(const Transform& Other) const
	{
		return !(*this == Other);
	}
}
