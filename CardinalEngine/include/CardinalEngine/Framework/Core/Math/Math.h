#pragma once
#include "../Definitions.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

#include <type_traits>
#include <cmath>

namespace CE
{
	class Math
	{
	public:
		// Deleted Constructor, only Static Functions
		Math() = delete;

		// Converts Degrees to Radians
		template<typename T>
		static T Radians(T Degrees)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Degrees * static_cast<T>(0.01745329251994329576923690768489);
		}

		// Converts a Vec2 from Degrees to Radians
		template<typename T>
		static Vec2<T> Radians(const Vec2<T>& Degrees)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec2<T>(Radians(Degrees.X), Radians(Degrees.Y));
		}

		// Converts a Vec3 from Degrees to Radians
		template<typename T>
		static Vec3<T> Radians(const Vec3<T>& Degrees)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec3<T>(Radians(Degrees.X), Radians(Degrees.Y), Radians(Degrees.Z));
		}

		// Converts a Vec4 from Degrees to Radians
		template<typename T>
		static Vec4<T> Radians(const Vec4<T>& Degrees)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec4<T>(Radians(Degrees.X), Radians(Degrees.Y), Radians(Degrees.Z), Radians(Degrees.W));
		}

		// Converts from Radians to Degrees
		template<typename T>
		static T Degrees(T Radians)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Radians * static_cast<T>(57.295779513082320876798154814105);
		}

		// Converts a Vec2 from Radians to Degrees
		template<typename T>
		static Vec2<T> Degrees(const Vec2<T>& Radians)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec2<T>(Degrees(Radians.X), Degrees(Radians.Y));
		}

		// Converts a Vec3 from Radians to Degrees
		template<typename T>
		static Vec3<T> Degrees(const Vec3<T>& Radians)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec3<T>(Degrees(Radians.X), Degrees(Radians.Y), Degrees(Radians.Z));
		}

		// Converts a Vec4 from Radians to Degrees
		template<typename T>
		static Vec4<T> Degrees(const Vec4<T>& Radians)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Vec4<T>(Degrees(Radians.X), Degrees(Radians.Y), Degrees(Radians.Z); Degrees(Radians.W));
		}

		// Clamps a Value
		template<typename T>
		static T Clamp(T Val, T Min, T Max)
		{
			return std::min(std::max(Val, Min), Max);
		}

		// Clamps a Vec2
		template<typename T>
		static Vec2<T> Clamp(const Vec2<T>& Val, const Vec2<T>& Min, const Vec2<T>& Max)
		{
			return Vec2<T>(Clamp(Val.X, Min.X, Max.X), Clamp(Val.Y, Min.Y, Max.Y));
		}

		// Clamps a Vec3
		template<typename T>
		static const Vec3<T>& Clamp(const Vec3<T>& Val, const Vec3<T>& Min, const Vec3<T>& Max)
		{
			return Vec3<T>(Clamp(Val.X, Min.X, Max.X), Clamp(Val.Y, Min.Y, Max.Y), Clamp(Val.Z, Min.Z, Max.Z));
		}

		// Clamps a Vec4
		template<typename T>
		static const Vec4<T>& Clamp(const Vec4<T>& Val, const Vec4<T>& Min, const Vec4<T>& Max)
		{
			return Vec4<T>(Clamp(Val.X, Min.X, Max.X), Clamp(Val.Y, Min.Y, Max.Y), Clamp(Val.Z, Min.Z, Max.Z), Clamp(Val.W, Min.W, Max.W));
		}

		template<typename T>
		static bool NearlyEqual(T Val, T Equal, T Range = 0.001)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return Val < Equal + Range && Val > Equal - Range;
		}

		template<typename T>
		static bool NearlyEqual(const Vec2<T>& Val, const Vec2<T>& Equal, T Range = 0.001)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return NearlyEqual(Val.X, Equal.X, Range) && NearlyEqual(Val.Y, Equal.Y, Range);
		}

		template<typename T>
		static bool NearlyEqual(const Vec3<T>& Val, const Vec3<T>& Equal, T Range = 0.001)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return NearlyEqual(Val.X, Equal.X, Range) && NearlyEqual(Val.Y, Equal.Y, Range) && NearlyEqual(Val.Z, Equal.Z, Range);
		}

		template<typename T>
		static bool NearlyEqual(const Vec4<T>& Val, const Vec4<T>& Equal, T Range = 0.001)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			return NearlyEqual(Val.X, Equal.X, Range) && NearlyEqual(Val.Y, Equal.Y, Range) && NearlyEqual(Val.Z, Equal.Z, Range) && NearlyEqual(Val.W, Equal.W, Range);
		}

		template<typename T>
		static T InvSqrt(T Val)
		{
			static_assert(std::is_floating_point_v<T>, "T has to be a floating point value");
			union {
				T f;
				uint32_t i;
			} conv;

			float x2;
			const float threehalfs = 1.5F;

			x2 = Val * 0.5F;
			conv.f = Val;
			conv.i = 0x5f3759df - (conv.i >> 1);
			conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
			return conv.f;
		}
	};
}
