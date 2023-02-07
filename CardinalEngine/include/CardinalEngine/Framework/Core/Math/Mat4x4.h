#pragma once
#include "../Definitions.h"
#include "../Logging/Log.h"
#include "Vec4.h"
#include "Quaternion.h"

#include <stdlib.h>
#include <limits>

namespace CE
{
	template<typename T>
	class Mat4x4
	{
	public:
		// Returns the Identity Matrix
		static Mat4x4 Identity() { return Mat4x4(1); }

		// Returns the Mat4x4 from a Quaternion
		static Mat4x4 FromQuaternion(const Quat<T>& Quat)
		{
			Mat4x4<T> Result = Mat4x4<T>::Identity();
			T qxx(Quat.X * Quat.X);
			T qyy(Quat.Y * Quat.Y);
			T qzz(Quat.Z * Quat.Z);
			T qxz(Quat.X * Quat.Z);
			T qxy(Quat.X * Quat.Y);
			T qyz(Quat.Y * Quat.Z);
			T qwx(Quat.W * Quat.X);
			T qwy(Quat.W * Quat.Y);
			T qwz(Quat.W * Quat.Z);

			Result[0][0] = T(1) - T(2) * (qyy + qzz);
			Result[0][1] = T(2) * (qxy + qwz);
			Result[0][2] = T(2) * (qxz - qwy);

			Result[1][0] = T(2) * (qxy - qwz);
			Result[1][1] = T(1) - T(2) * (qxx + qzz);
			Result[1][2] = T(2) * (qyz + qwx);

			Result[2][0] = T(2) * (qxz + qwy);
			Result[2][1] = T(2) * (qyz - qwx);
			Result[2][2] = T(1) - T(2) * (qxx + qyy);

			return Result;
		}

		// Returns the Look At Matrix Right Hand sided
		static Mat4x4 LookAtRH(const Vec3<T>& FromPos, const Vec3<T>& ToPos, const Vec3<T>& Up)
		{
			const Vec3<T> f = (ToPos - FromPos).Normalize();
			const Vec3<T> s = f.Cross(Up).Normalize();
			const Vec3<T> u = s.Cross(f);

			Mat4x4 Result(1);
			Result[0][0] = s.X;
			Result[1][0] = s.Y;
			Result[2][0] = s.Z;
			Result[0][1] = u.X;
			Result[1][1] = u.Y;
			Result[2][1] = u.Z;
			Result[0][2] = -f.X;
			Result[1][2] = -f.Y;
			Result[2][2] = -f.Z;
			Result[3][0] = -(s.Dot(FromPos));
			Result[3][1] = -(u.Dot(FromPos));
			Result[3][2] = f.Dot(FromPos);
			return Result;
		}

		// Returns the Look At Matrix Left Hand sided
		static Mat4x4 LookAtLH(const Vec3<T>& FromPos, const Vec3<T>& ToPos, const Vec3<T>& Up)
		{
			const Vec3<T> f = (ToPos - FromPos).Normalize();
			const Vec3<T> s = Up.Cross(f).Normalize();
			const Vec3<T> u = f.Cross(s);

			Mat4x4 Result(1);
			Result[0][0] = s.X;
			Result[1][0] = s.Y;
			Result[2][0] = s.Z;
			Result[0][1] = u.X;
			Result[1][1] = u.Y;
			Result[2][1] = u.Z;
			Result[0][2] = f.X;
			Result[1][2] = f.Y;
			Result[2][2] = f.Z;
			Result[3][0] = -(s.Dot(FromPos));
			Result[3][1] = -(u.Dot(FromPos));
			Result[3][2] = -(f.Dot(FromPos));
			return Result;
		}

		static Mat4x4 OrthoLH_ZO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(1);
			Result[0][0] = static_cast<T>(2) / (Right - Left);
			Result[1][1] = static_cast<T>(2) / (Top - Bottom);
			Result[2][2] = static_cast<T>(1) / (Far - Near);
			Result[3][0] = -(Right + Left) / (Right - Left);
			Result[3][1] = -(Top + Bottom) / (Top - Bottom);
			Result[3][2] = -Near / (Far - Near);
			return Result;
		}

		static Mat4x4 OrthoLH_NO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(1);
			Result[0][0] = static_cast<T>(2) / (Right - Left);
			Result[1][1] = static_cast<T>(2) / (Top - Bottom);
			Result[2][2] = static_cast<T>(2) / (Far - Near);
			Result[3][0] = -(Right + Left) / (Right - Left);
			Result[3][1] = -(Top + Bottom) / (Top - Bottom);
			Result[3][2] = -(Far + Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 OrthoRH_ZO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(1);
			Result[0][0] = static_cast<T>(2) / (Right - Left);
			Result[1][1] = static_cast<T>(2) / (Top - Bottom);
			Result[2][2] = -static_cast<T>(1) / (Far - Near);
			Result[3][0] = -(Right + Left) / (Right - Left);
			Result[3][1] = -(Top + Bottom) / (Top - Bottom);
			Result[3][2] = -Near / (Far - Near);
			return Result;
		}

		static Mat4x4 OrthoRH_NO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(1);
			Result[0][0] = static_cast<T>(2) / (Right - Left);
			Result[1][1] = static_cast<T>(2) / (Top - Bottom);
			Result[2][2] = -static_cast<T>(2) / (Far - Near);
			Result[3][0] = -(Right + Left) / (Right - Left);
			Result[3][1] = -(Top + Bottom) / (Top - Bottom);
			Result[3][2] = -(Far + Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 FrustumLH_ZO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(0);
			Result[0][0] = (static_cast<T>(2) * Near) / (Right - Left);
			Result[1][1] = (static_cast<T>(2) * Near) / (Top - Bottom);
			Result[2][0] = -(Right + Left) / (Right - Left);
			Result[2][1] = -(Top + Bottom) / (Top - Bottom);
			Result[2][2] = Far / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 FrustumLH_NO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(0);
			Result[0][0] = (static_cast<T>(2) * Near) / (Right - Left);
			Result[1][1] = (static_cast<T>(2) * Near) / (Top - Bottom);
			Result[2][0] = -(Right + Left) / (Right - Left);
			Result[2][1] = -(Top + Bottom) / (Top - Bottom);
			Result[2][2] = (Far + Near) / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 FrustumRH_ZO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(0);
			Result[0][0] = (static_cast<T>(2) * Near) / (Right - Left);
			Result[1][1] = (static_cast<T>(2) * Near) / (Top - Bottom);
			Result[2][0] = (Right + Left) / (Right - Left);
			Result[2][1] = (Top + Bottom) / (Top - Bottom);
			Result[2][2] = Far / (Near - Far);
			Result[2][3] = static_cast<T>(-1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 FrustumRH_NO(T Left, T Right, T Bottom, T Top, T Near, T Far)
		{
			Mat4x4 Result(0);
			Result[0][0] = (static_cast<T>(2) * Near) / (Right - Left);
			Result[1][1] = (static_cast<T>(2) * Near) / (Top - Bottom);
			Result[2][0] = (Right + Left) / (Right - Left);
			Result[2][1] = (Top + Bottom) / (Top - Bottom);
			Result[2][2] = -(Far + Near) / (Far - Near);
			Result[2][3] = static_cast<T>(-1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveLH_ZO(T Fov, T Aspect, T Near, T Far)
		{
			CE_CHECK(std::abs(Aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

			T const tanHalfFov = std::tan(Fov / static_cast<T>(2));

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = static_cast<T>(1) / (Aspect * tanHalfFov);
			Result[1][1] = static_cast<T>(1) / (tanHalfFov);
			Result[2][2] = Far / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveLH_NO(T Fov, T Aspect, T Near, T Far)
		{
			CE_CHECK(std::abs(Aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

			T const tanHalfFov = std::tan(Fov / static_cast<T>(2));

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = static_cast<T>(1) / (Aspect * tanHalfFov);
			Result[1][1] = static_cast<T>(1) / (tanHalfFov);
			Result[2][2] = (Far + Near) / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveRH_ZO(T Fov, T Aspect, T Near, T Far)
		{
			CE_CHECK(std::abs(Aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

			T const tanHalfFov = std::tan(Fov / static_cast<T>(2));

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = static_cast<T>(1) / (Aspect * tanHalfFov);
			Result[1][1] = static_cast<T>(1) / (tanHalfFov);
			Result[2][2] = Far / (Near - Far);
			Result[2][3] = -static_cast<T>(1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveRH_NO(T Fov, T Aspect, T Near, T Far)
		{
			CE_CHECK(std::abs(Aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

			T const tanHalfFov = std::tan(Fov / static_cast<T>(2));

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = static_cast<T>(1) / (Aspect * tanHalfFov);
			Result[1][1] = static_cast<T>(1) / (tanHalfFov);
			Result[2][2] = -(Far + Near) / (Far - Near);
			Result[2][3] = -static_cast<T>(1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveFovLH_ZO(T Fov, T width, T height, T Near, T Far)
		{
			CE_CHECK(width > static_cast<T>(0));
			CE_CHECK(height > static_cast<T>(0));
			CE_CHECK(Fov > static_cast<T>(0));

			T const rad = Fov;
			T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
			T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = w;
			Result[1][1] = h;
			Result[2][2] = Far / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveFovLH_NO(T Fov, T width, T height, T Near, T Far)
		{
			CE_CHECK(width > static_cast<T>(0));
			CE_CHECK(height > static_cast<T>(0));
			CE_CHECK(Fov > static_cast<T>(0));

			T const rad = Fov;
			T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
			T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = w;
			Result[1][1] = h;
			Result[2][2] = (Far + Near) / (Far - Near);
			Result[2][3] = static_cast<T>(1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveFovRH_ZO(T Fov, T width, T height, T Near, T Far)
		{
			CE_CHECK(width > static_cast<T>(0));
			CE_CHECK(height > static_cast<T>(0));
			CE_CHECK(Fov > static_cast<T>(0));

			T const rad = Fov;
			T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
			T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = w;
			Result[1][1] = h;
			Result[2][2] = Far / (Near - Far);
			Result[2][3] = -static_cast<T>(1);
			Result[3][2] = -(Far * Near) / (Far - Near);
			return Result;
		}

		static Mat4x4 PerspectiveFovRH_NO(T Fov, T width, T height, T Near, T Far)
		{
			CE_CHECK(width > static_cast<T>(0));
			CE_CHECK(height > static_cast<T>(0));
			CE_CHECK(Fov > static_cast<T>(0));

			T const rad = Fov;
			T const h = std::cos(static_cast<T>(0.5) * rad) / std::sin(static_cast<T>(0.5) * rad);
			T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

			Mat4x4 Result(static_cast<T>(0));
			Result[0][0] = w;
			Result[1][1] = h;
			Result[2][2] = -(Far + Near) / (Far - Near);
			Result[2][3] = -static_cast<T>(1);
			Result[3][2] = -(static_cast<T>(2) * Far * Near) / (Far - Near);
			return Result;
		}

	public:
		// Constructor
		Mat4x4() = default;

		// Copy Constructor
		Mat4x4(const Mat4x4& Other) = default;

		// Move Constructor
		Mat4x4(Mat4x4&& Other) = default;

		// Constructor with Single Value
		Mat4x4(const T& Val) : Row0(Val, 0, 0, 0), Row1(0, Val, 0, 0), Row2(0, 0, Val, 0), Row3(0, 0, 0, Val) {}

		// Constructor with Vec4 as Rows
		Mat4x4(const Vec4<T>& Row0, const Vec4<T>& Row1, const Vec4<T>& Row2, const Vec4<T>& Row3) : Row0(Row0), Row1(Row1), Row2(Row2), Row3(Row3) {}

		// Constructor with Rows
		Mat4x4(const T& Val00, const T& Val01, const T& Val02, const T& Val03,
			const T& Val10, const T& Val11, const T& Val12, const T& Val13,
			const T& Val20, const T& Val21, const T& Val22, const T& Val23,
			const T& Val30, const T& Val31, const T& Val32, const T& Val33)
			: Row0(Val00, Val01, Val02, Val03), Row1(Val10, Val11, Val12, Val13), Row2(Val20, Val21, Val22, Val23), Row3(Val30, Val31, Val32, Val33) {}

	public:
		// Returns the Inverted Matrix
		Mat4x4 Inverse() const
		{
			T Coef00 = Row2[2] * Row3[3] - Row3[2] * Row2[3];
			T Coef02 = Row1[2] * Row3[3] - Row3[2] * Row1[3];
			T Coef03 = Row1[2] * Row2[3] - Row2[2] * Row1[3];

			T Coef04 = Row2[1] * Row3[3] - Row3[1] * Row2[3];
			T Coef06 = Row1[1] * Row3[3] - Row3[1] * Row1[3];
			T Coef07 = Row1[1] * Row2[3] - Row2[1] * Row1[3];

			T Coef08 = Row2[1] * Row3[2] - Row3[1] * Row2[2];
			T Coef10 = Row1[1] * Row3[2] - Row3[1] * Row1[2];
			T Coef11 = Row1[1] * Row2[2] - Row2[1] * Row1[2];

			T Coef12 = Row2[0] * Row3[3] - Row3[0] * Row2[3];
			T Coef14 = Row1[0] * Row3[3] - Row3[0] * Row1[3];
			T Coef15 = Row1[0] * Row2[3] - Row2[0] * Row1[3];

			T Coef16 = Row2[0] * Row3[2] - Row3[0] * Row2[2];
			T Coef18 = Row1[0] * Row3[2] - Row3[0] * Row1[2];
			T Coef19 = Row1[0] * Row2[2] - Row2[0] * Row1[2];

			T Coef20 = Row2[0] * Row3[1] - Row3[0] * Row2[1];
			T Coef22 = Row1[0] * Row3[1] - Row3[0] * Row1[1];
			T Coef23 = Row1[0] * Row2[1] - Row2[0] * Row1[1];

			Vec4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
			Vec4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
			Vec4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
			Vec4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
			Vec4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
			Vec4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

			Vec4<T> Vec0(Row1[0], Row0[0], Row0[0], Row0[0]);
			Vec4<T> Vec1(Row1[1], Row0[1], Row0[1], Row0[1]);
			Vec4<T> Vec2(Row1[2], Row0[2], Row0[2], Row0[2]);
			Vec4<T> Vec3(Row1[3], Row0[3], Row0[3], Row0[3]);

			Vec4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			Vec4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			Vec4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			Vec4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			Vec4<T> SignA(+1, -1, +1, -1);
			Vec4<T> SignB(-1, +1, -1, +1);
			Mat4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

			Vec4<T> TmpRow0(Inverse.Row0[0], Inverse.Row1[0], Inverse.Row2[0], Inverse.Row3[0]);

			Vec4<T> Dot0(Row0 * TmpRow0);
			T Dot1 = (Dot0.X + Dot0.Y) + (Dot0.Z + Dot0.W);

			T OneOverDeterminant = static_cast<T>(1) / Dot1;

			return Inverse * OneOverDeterminant;
		}

		// Translates the Vec3
		template<typename O>
		Mat4x4 Translate(const Vec3<O>& Translation) const
		{
			Mat4x4 Result(*this);
			Result[3] = Result[0] * Translation[0] + Result[1] * Translation[1] + Result[2] * Translation[2] + Result[3];
			return Result;
		}

		// Rotates the Vec3
		Mat4x4 Rotate(float Angle, const Vec3<T>& Axis) const
		{
			T const a = Angle;
			T const c = std::cos(a);
			T const s = std::sin(a);

			Vec3<T> axis(Axis.Normalize());
			Vec3<T> temp(axis * (T(1) - c));

			Mat4x4 Rotate;
			Rotate[0][0] = c + temp[0] * axis[0];
			Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
			Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

			Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
			Rotate[1][1] = c + temp[1] * axis[1];
			Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

			Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
			Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
			Rotate[2][2] = c + temp[2] * axis[2];

			Mat4x4 Result;
			Result[0] = Row0 * Rotate[0][0] + Row1 * Rotate[0][1] + Row2 * Rotate[0][2];
			Result[1] = Row0 * Rotate[1][0] + Row1 * Rotate[1][1] + Row2 * Rotate[1][2];
			Result[2] = Row0 * Rotate[2][0] + Row1 * Rotate[2][1] + Row2 * Rotate[2][2];
			Result[3] = Row3;
			return Result;
		}

		// Scales the Vec3
		Mat4x4 Scale(const Vec3<T>& Scale) const
		{
			Mat4x4 Result;
			Result[0] = Row0 * Scale.X;
			Result[1] = Row1 * Scale.Y;
			Result[2] = Row2 * Scale.Z;
			Result[3] = Row3;
			return Result;
		}

		Quat<T> ToQuaternion() const
		{
#if 1
			Vec3f Result[3];
			Result[0] = Vec3f(Row0.X, Row0.Y, Row0.Z);
			Result[1] = Vec3f(Row1.X, Row1.Y, Row1.Z);
			Result[2] = Vec3f(Row2.X, Row2.Y, Row2.Z);

			float fourXSquaredMinus1 = Result[0][0] - Result[1][1] - Result[2][2];
			float fourYSquaredMinus1 = Result[1][1] - Result[0][0] - Result[2][2];
			float fourZSquaredMinus1 = Result[2][2] - Result[0][0] - Result[1][1];
			float fourWSquaredMinus1 = Result[0][0] + Result[1][1] + Result[2][2];

			int biggestIndex = 0;
			float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
			if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourXSquaredMinus1;
				biggestIndex = 1;
			}
			if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourYSquaredMinus1;
				biggestIndex = 2;
			}
			if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourZSquaredMinus1;
				biggestIndex = 3;
			}

			float biggestVal = sqrt(fourBiggestSquaredMinus1 + static_cast<float>(1)) * static_cast<float>(0.5);
			float mult = static_cast<float>(0.25) / biggestVal;

			switch (biggestIndex)
			{
			case 0:
				return Quaternion((Result[1][2] - Result[2][1]) * mult, (Result[2][0] - Result[0][2]) * mult, (Result[0][1] - Result[1][0]) * mult, biggestVal);
			case 1:
				return Quaternion(biggestVal, (Result[0][1] + Result[1][0]) * mult, (Result[2][0] + Result[0][2]) * mult, (Result[1][2] - Result[2][1]) * mult);
			case 2:
				return Quaternion((Result[0][1] + Result[1][0]) * mult, biggestVal, (Result[1][2] + Result[2][1]) * mult, (Result[2][0] - Result[0][2]) * mult);
			case 3:
				return Quaternion((Result[2][0] + Result[0][2]) * mult, (Result[1][2] + Result[2][1]) * mult, biggestVal, (Result[0][1] - Result[1][0]) * mult);
			default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
				CE_CHECK(false);
				return Quaternion(1, 0, 0, 0);
			}
#else
			const Mat4x4& Mat = *this;

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
			return Result;
#endif
		}

	public:
		const Vec4<T>& operator[](int Row) const
		{
			CE_CHECK(Row >= 0 && Row <= 3);
			switch (Row)
			{
			default:
			case 0: return Row0;
			case 1: return Row1;
			case 2: return Row2;
			case 3: return Row3;
			}
		}

		Vec4<T>& operator[](int Row)
		{
			CE_CHECK(Row >= 0 && Row <= 3);
			switch (Row)
			{
			default:
			case 0: return Row0;
			case 1: return Row1;
			case 2: return Row2;
			case 3: return Row3;
			}
		}

		Mat4x4& operator=(const Mat4x4& Other) = default;

		Mat4x4& operator=(Mat4x4&& Other) = default;

		Mat4x4 operator+() const
		{
			return *this;
		}

		Mat4x4 operator-() const
		{
			return Mat4x4(-Row0, -Row1, -Row2, -Row3);
		}

		template<typename O>
		Mat4x4& operator+=(const O& Val)
		{
			Row0 += Val;
			Row1 += Val;
			Row2 += Val;
			Row3 += Val;
			return *this;
		}

		template<typename O>
		Mat4x4& operator+=(const Mat4x4<O>& Val)
		{
			Row0 += Val.Row0;
			Row1 += Val.Row1;
			Row2 += Val.Row2;
			Row3 += Val.Row3;
			return *this;
		}

		template<typename O>
		Mat4x4& operator-=(const O& Val)
		{
			Row0 -= Val;
			Row1 -= Val;
			Row2 -= Val;
			Row3 -= Val;
			return *this;
		}

		template<typename O>
		Mat4x4& operator-=(const Mat4x4<O>& Val)
		{
			Row0 -= Val.Row0;
			Row1 -= Val.Row1;
			Row2 -= Val.Row2;
			Row3 -= Val.Row3;
			return *this;
		}

		template<typename O>
		Mat4x4& operator*=(const O& Val)
		{
			Row0 *= Val;
			Row1 *= Val;
			Row2 *= Val;
			Row3 *= Val;
			return *this;
		}

		template<typename O>
		Mat4x4& operator*=(const Mat4x4<O>& Val)
		{
			Mat4x4<T> Result;
			Result[0] = Row0 * Val.Row0[0] + Row1 * Val.Row0[1] + Row2 * Val.Row0[2] + Row3 * Val.Row0[3];
			Result[1] = Row0 * Val.Row1[0] + Row1 * Val.Row1[1] + Row2 * Val.Row1[2] + Row3 * Val.Row1[3];
			Result[2] = Row0 * Val.Row2[0] + Row1 * Val.Row2[1] + Row2 * Val.Row2[2] + Row3 * Val.Row2[3];
			Result[3] = Row0 * Val.Row3[0] + Row1 * Val.Row3[1] + Row2 * Val.Row3[2] + Row3 * Val.Row3[3];
			return *this = Result;
		}

		template<typename O>
		Mat4x4& operator/=(const O& Val)
		{
			Row0 /= Val;
			Row1 /= Val;
			Row2 /= Val;
			Row3 /= Val;
			return *this;
		}

		template<typename O>
		Mat4x4& operator/=(const Mat4x4<O>& Val)
		{
			return *this *= Val.Inverse();
		}

		Mat4x4& operator++()
		{
			Row0++;
			Row1++;
			Row2++;
			Row3++;
			return *this;
		}

		Mat4x4 operator++(int)
		{
			Mat4x4 Result = *this;
			++Row0;
			++Row1;
			++Row2;
			++Row3;
			return Result;
		}

		Mat4x4& operator--()
		{
			Row0--;
			Row1--;
			Row2--;
			Row3--;
			return *this;
		}

		Mat4x4 operator--(int)
		{
			Mat4x4 Result = *this;
			--Row0;
			--Row1;
			--Row2;
			--Row3;
			return Result;
		}

		template<typename O>
		Mat4x4 operator+(const O& Val) const
		{
			return Mat4x4(*this) += Val;
		}

		template<typename O>
		Mat4x4 operator+(const Mat4x4<O>& Val) const
		{
			return Mat4x4(*this) += Val;
		}

		template<typename O>
		Mat4x4 operator-(const O& Val) const
		{
			return Mat4x4(*this) -= Val;
		}

		template<typename O>
		Mat4x4 operator-(const Mat4x4<O>& Val) const
		{
			return Mat4x4(*this) -= Val;
		}

		template<typename O>
		Mat4x4 operator*(const O& Val) const
		{
			return Mat4x4(*this) *= Val;
		}

		template<typename O>
		Mat4x4 operator*(const Mat4x4<O>& Val) const
		{
			return Mat4x4(*this) *= Val;
		}

		template<typename O>
		Mat4x4 operator/(const O& Val) const
		{
			return Mat4x4(*this) /= Val;
		}

		template<typename O>
		Mat4x4 operator/(const Mat4x4<O>& Val) const
		{
			return Mat4x4(*this) /= Val;
		}

		template<typename O>
		bool operator==(const Mat4x4<O>& Other) const
		{
			return Row0 == Other.Row0, Row1 == Other.Row1, Row2 == Other.Row2, Row3 == Other.Row3;
		}

		template<typename O>
		bool operator!=(const Mat4x4<O>& Other) const
		{
			return !(*this == Other);
		}

	public:
		Vec4<T> Row0;
		Vec4<T> Row1;
		Vec4<T> Row2;
		Vec4<T> Row3;
	};

	typedef Mat4x4<float> Mat4x4f;
	typedef Mat4x4<int> Mat4x4i;
	typedef Mat4x4<unsigned int> Mat4x4ui;
}
