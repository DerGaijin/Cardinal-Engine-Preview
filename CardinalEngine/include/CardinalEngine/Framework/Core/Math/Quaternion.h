#pragma once
#include "../Definitions.h"
#include "Vec4.h"
#include "Math.h"

namespace CE
{
	template<typename T>
	class Quat
	{
	public:
		// Returns a Quaternion from Euler Angles
		static Quat FromEuler(const Vec3<T>& RadianEulerAngles)
		{
			Vec3<T> Half = RadianEulerAngles * T(0.5);
			Vec3<T> c = Vec3<T>(cos(Half.X), cos(Half.Y), cos(Half.Z));
			Vec3<T> s = Vec3<T>(sin(Half.X), sin(Half.Y), sin(Half.Z));

			return Quat(
				s.X * c.Y * c.Z - c.X * s.Y * s.Z,
				c.X * s.Y * c.Z + s.X * c.Y * s.Z,
				c.X * c.Y * s.Z - s.X * s.Y * c.Z,
				c.X * c.Y * c.Z + s.X * s.Y * s.Z
			);
		}

		// Returns a Quaternion from Axis and Angle
		static Quat FromAxisAngle(const Vec3f& Axis, float Angle)
		{
			float HalfAngle = Angle * 0.5f;
			float s = std::sin(HalfAngle);

			return Quat(
				Axis.X * s,
				Axis.Y * s,
				Axis.Z * s,
				std::cos(HalfAngle)
			);
		}

		// Returns a Quaternion representing the LookAt direction
		static Quat LookAt(const Vec3f& Source, const Vec3f& Target, const Vec3f& Up)
		{
			Mat4x4f Mat = Mat4x4f::LookAtLH(Source, Target, Up);
			Mat = Mat.Inverse();
			Mat = Mat.Rotate(Math::Radians(180.0f), Vec3f(0.0f, 1.0f, 1.0f));

			//const MeReal *const t = (MeReal *) tm;
			float	s;

			// Check diagonal (trace)
			const float tr = Mat[0][0] + Mat[1][1] + Mat[2][2];

			Quaternion Result;
			if (tr > 0.0f)
			{
				float InvS = Math::InvSqrt(tr + 1.f);
				Result.W = 0.5f * (1.f / InvS);
				s = 0.5f * InvS;

				Result.X = (Mat[1][2] - Mat[2][1]) * s;
				Result.Y = (Mat[2][0] - Mat[0][2]) * s;
				Result.Z = (Mat[0][1] - Mat[1][0]) * s;
			}
			else
			{
				// diagonal is negative
				int32 i = 0;

				if (Mat[1][1] > Mat[0][0])
					i = 1;

				if (Mat[2][2] > Mat[i][i])
					i = 2;

				static const int32 nxt[3] = { 1, 2, 0 };
				const int32 j = nxt[i];
				const int32 k = nxt[j];

				s = Mat[i][i] - Mat[j][j] - Mat[k][k] + 1.0f;

				float InvS = Math::InvSqrt(s);

				float qt[4];
				qt[i] = 0.5f * (1.f / InvS);

				s = 0.5f * InvS;

				qt[3] = (Mat[j][k] - Mat[k][j]) * s;
				qt[j] = (Mat[i][j] + Mat[j][i]) * s;
				qt[k] = (Mat[i][k] + Mat[k][i]) * s;

				Result.X = qt[0];
				Result.Y = qt[1];
				Result.Z = qt[2];
				Result.W = qt[3];
			}
			return Result.Normalize();
		}

		static Quat LookAtSafe(const Vec3f& Source, const Vec3f& Target, const Vec3f& Up)
		{
			Vec3f Direction = Target - Source;
			float DirectionLength = Direction.Length();

			// Check if the direction is valid; Also deals with NaN
			if (!(DirectionLength > 0.0001))
			{
				return Quaternion(1, 0, 0, 0); // Just return identity
			}

			// Normalize direction
			Direction /= DirectionLength;

			// Is the normal up (nearly) parallel to direction?
			if (std::abs(Direction.Dot(Up)) > 0.9999f)
			{
				// Use alternative up
				Vec3f AlternativeUp = Up + 0.0001f;
				return LookAt(Source, Target, AlternativeUp);
			}
			else
			{
				return LookAt(Source, Target, Up);
			}
		}

	public:
		// Constructor
		Quat() = default;

		// Constructor with X, Y, Z and W
		Quat(T X, T Y, T Z, T W) : X(X), Y(Y), Z(Z), W(W) {}

		// Constructor with a Vec4 as X, Y, Z and W
		Quat(const Vec4<T>& XYZW) : X(XYZW.X), Y(XYZW.Y), Z(XYZW.Z), W(XYZW.W) {}

		// Constructor with a Vec3 as X, Y and Z and W
		Quat(const Vec3<T>& XYZ, T W) : X(XYZ.X), Y(XYZ.Y), Z(XYZ.Z), W(W) {}

		// Constructor with X and a Vec3 as Y, Z and W
		Quat(T X, const Vec3<T>& YZW) : X(X), Y(YZW.X), Z(YZW.Y), W(YZW.Z) {}

		// Constructor with a Vec2 as XY and a Vec2 as Z and W
		Quat(const Vec2<T>& XY, const Vec2<T>& ZW) : X(XY.X), Y(XY.Y), Z(ZW.X), W(ZW.Y) {}

	public:
		// Returns the Roll of the Quaternion
		T Roll() const
		{
			return static_cast<T>(std::atan2(static_cast<T>(2) * (X * Y + W * Z), W * W + X * X - Y * Y - Z * Z));
		}

		// Returns the Pitch of the Quaternion
		T Pitch() const
		{
			T const y = static_cast<T>(2) * (Y * Z + W * X);
			T const x = W * W - X * X - Y * Y + Z * Z;

			T Epsilon = std::numeric_limits<T>::epsilon();
			if (std::abs(x) <= Epsilon && std::abs(y) <= Epsilon)
			{
				return static_cast<T>(static_cast<T>(2) * std::atan2(X, W));
			}

			return static_cast<T>(std::atan2(y, x));
		}

		// Returns the Yaw of the Quaternion
		T Yaw() const
		{
			return asin(Math::Clamp(static_cast<T>(-2) * (X * Z - W * Y), static_cast<T>(-1), static_cast<T>(1)));
		}

		// Returns the Euler Angles of the Quaternion
		Vec3<T> ToEuler() const
		{
			return Vec3(Pitch(), Yaw(), Roll());
		}

		// Rotates a Vec3 around the Quaternion
		Vec3<T> Rotate(const Vec3<T>& Other) const
		{
			Vec3<T> QuatVec(X, Y, Z);
			Vec3<T> QuatVecC = QuatVec.Cross(Other);
			Vec3<T> QuatVecCC = QuatVec.Cross(QuatVecC);

			return Other + ((QuatVecC * W) + QuatVecCC) * static_cast<T>(2);
		}

		// Rotates a Vec4 around the Quaternion
		Vec4<T> Rotate(const Vec4<T>& Other) const
		{
			return Vec4<T>((*this) * Vec3(Other.X, Other.Y, Other.Z), Other.W);
		}

		Quat Conjugate() const
		{
			return Quat<T>(-X, -Y, -Z, W);
		}

		T Dot(const Quat<T>& Other) const
		{
			Vec4<T> Tmp(X * Other.X, Y * Other.Y, Z * Other.Z, W * Other.W);
			return (Tmp.X + Tmp.Y) + (Tmp.Z + Tmp.W);
		}

		Quat Inverse() const
		{
			return Conjugate() / Dot(*this);
		}

		T Length() const
		{
			return std::sqrt(Dot(*this));
		}

		Quat Normalize() const
		{
			float mag = std::sqrtf(W * W + X * X + Y * Y + Z * Z);
			return Quat(X / mag, Y / mag, Z / mag, W / mag);
		}

	public:
		Quat& operator+=(const Quat& Other)
		{
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			W += Other.W;
			return *this;
		}

		Quat& operator-=(const Quat& Other)
		{
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			W -= Other.W;
			return *this;
		}

		Quat& operator*=(const Quat& Other)
		{
			Quat Temp = *this;
			X = Temp.W * Other.X + Temp.X * Other.W + Temp.Y * Other.Z - Temp.Z * Other.Y;
			Y = Temp.W * Other.Y + Temp.Y * Other.W + Temp.Z * Other.X - Temp.X * Other.Z;
			Z = Temp.W * Other.Z + Temp.Z * Other.W + Temp.X * Other.Y - Temp.Y * Other.X;
			W = Temp.W * Other.W - Temp.X * Other.X - Temp.Y * Other.Y - Temp.Z * Other.Z;
			return *this;
		}

		Quat& operator*=(T Scalar)
		{
			X *= Scalar;
			Y *= Scalar;
			Z *= Scalar;
			W *= Scalar;
			return *this;
		}

		Quat& operator/=(T Scalar)
		{
			X /= Scalar;
			Y /= Scalar;
			Z /= Scalar;
			W /= Scalar;
			return *this;
		}

		Quat operator+() const
		{
			return *this;
		}

		Quat operator-() const
		{
			return Quat(-X, -Y, -Z, -W);
		}

		Quat operator+(const Quat& Other) const
		{
			return Quat(*this) += Other;
		}

		Quat operator-(const Quat& Other) const
		{
			return Quat(*this) -= Other;
		}

		Quat operator*(const Quat& Other) const
		{
			return Quat(*this) *= Other;
		}

		Quat operator/(T Scalar) const
		{
			return Quat(*this) /= Scalar;
		}

		bool operator==(const Quat& Other) const
		{
			return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
		}

		bool operator!=(const Quat& Other) const
		{
			return !(*this == Other);
		}

	public:
		T X = 0;
		T Y = 0;
		T Z = 0;
		T W = 1;
	};

	typedef Quat<float> Quaternion;
}
