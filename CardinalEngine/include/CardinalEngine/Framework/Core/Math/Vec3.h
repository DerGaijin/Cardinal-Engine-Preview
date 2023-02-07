#pragma once
#include "../Definitions.h"
#include "../Logging/Log.h"
#include "Vec2.h"

namespace CE
{
	template<typename T>
	class Vec3
	{
		static_assert(std::is_arithmetic_v<T>, "T has to be Arithmetic");
	public:
		// Constructor
		Vec3() = default;

		// Copy Constructor
		Vec3(const Vec3&) = default;

		// Move Constructor
		Vec3(Vec3&&) = default;

		// Constructor with single Value
		Vec3(const T& Val) : X(Val), Y(Val), Z(Val) {}

		// Constructor with X,Y and Z
		Vec3(const T& X, const T& Y, const T& Z) : X(X), Y(Y), Z(Z) {}

		// Constructor with XY and Z
		Vec3(const Vec2<T>& XY, const T& Z) : X(XY.X), Y(XY.Y), Z(Z) {}

		// Constructor with X and YZ
		Vec3(const T& X, const Vec2<T>& YZ) : X(X), Y(YZ.X), Z(YZ.Y) {}

	public:
		// Returns the Cross product with the Other Vec
		Vec3 Cross(const Vec3& Other) const
		{
			return Vec3(
				Y * Other.Z - Other.Y * Z,
				Z * Other.X - Other.Z * X,
				X * Other.Y - Other.X * Y);
		}

		// Returns the Distance with the Other Vec
		float Distance(const Vec3& Other) const
		{
			return Vec3(Other - *this).Length();
		}

		// Normalizes the Vec
		Vec3 Normalize() const
		{
			return *this * (1 / std::sqrt(Dot(*this)));
		}

		// Returns the Dot product with the other Vec
		float Dot(const Vec3& Other) const
		{
			Vec3 Tmp(*this * Other);
			return Tmp.X + Tmp.Y + Tmp.Z;
		}

		// Returns the length of the Vec
		float Length() const
		{
			return std::sqrt(Dot(*this));
		}

	public:
		const T& operator[](int Comp) const
		{
			CE_CHECK(Comp >= 0 && Comp <= 2);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
		}

		T& operator[](int Comp)
		{
			CE_CHECK(Comp >= 0 && Comp <= 2);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
		}

		template<typename O>
		Vec3& operator=(const O& Val)
		{
			X = Y = Z = Val;
			return *this;
		}

		Vec3& operator=(const Vec3&) = default;

		Vec3& operator=(Vec3&&) = default;

		Vec3 operator+() const
		{
			return *this;
		}

		Vec3 operator-() const
		{
			return Vec3(-X, -Y, -Z);
		}

		template<typename O>
		Vec3& operator+=(const O& Val)
		{
			X += Val;
			Y += Val;
			Z += Val;
			return *this;
		}

		template<typename O>
		Vec3& operator+=(const Vec3<O>& Val)
		{
			X += Val.X;
			Y += Val.Y;
			Z += Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator+(const O& Val) const
		{
			return Vec3(*this) += Val;
		}

		template<typename O>
		Vec3 operator+(const Vec3<O>& Val) const
		{
			return Vec3(*this) += Val;
		}

		template<typename O>
		Vec3& operator-=(const O& Val)
		{
			X -= Val;
			Y -= Val;
			Z -= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator-=(const Vec3<O>& Val)
		{
			X -= Val.X;
			Y -= Val.Y;
			Z -= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator-(const O& Val) const
		{
			return Vec3(*this) -= Val;
		}

		template<typename O>
		Vec3 operator-(const Vec3<O>& Val) const
		{
			return Vec3(*this) -= Val;
		}

		template<typename O>
		Vec3& operator*=(const O& Val)
		{
			X *= Val;
			Y *= Val;
			Z *= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator*=(const Vec3<O>& Val)
		{
			X *= Val.X;
			Y *= Val.Y;
			Z *= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator*(const O& Val) const
		{
			return Vec3(*this) *= Val;
		}

		template<typename O>
		Vec3 operator*(const Vec3<O>& Val) const
		{
			return Vec3(*this) *= Val;
		}

		template<typename O>
		Vec3& operator/=(const O& Val)
		{
			X /= Val;
			Y /= Val;
			Z /= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator/=(const Vec3<O>& Val)
		{
			X /= Val.X;
			Y /= Val.Y;
			Z /= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator/(const O& Val) const
		{
			return Vec3(*this) /= Val;
		}

		template<typename O>
		Vec3 operator/(const Vec3<O>& Val) const
		{
			return Vec3(*this) /= Val;
		}

		template<typename O>
		Vec3& operator%=(const O& Val)
		{
			X %= Val;
			Y %= Val;
			Z %= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator%=(const Vec3<O>& Val)
		{
			X %= Val.X;
			Y %= Val.Y;
			Z %= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator%(const O& Val) const
		{
			return Vec3(*this) %= Val;
		}

		template<typename O>
		Vec3 operator%(const Vec3<O>& Val) const
		{
			return Vec3(*this) %= Val;
		}

		template<typename O>
		Vec3& operator&=(const O& Val)
		{
			X &= Val;
			Y &= Val;
			Z &= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator&=(const Vec3<O>& Val)
		{
			X &= Val.X;
			Y &= Val.Y;
			Z &= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator&(const O& Val) const
		{
			return Vec3(*this) &= Val;
		}

		template<typename O>
		Vec3 operator&(const Vec3<O>& Val) const
		{
			return Vec3(*this) &= Val;
		}

		template<typename O>
		Vec3& operator|=(const O& Val)
		{
			X |= Val;
			Y |= Val;
			Z |= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator|=(const Vec3<O>& Val)
		{
			X |= Val.X;
			Y |= Val.Y;
			Z |= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator|(const O& Val) const
		{
			return Vec3(*this) |= Val;
		}

		template<typename O>
		Vec3 operator|(const Vec3<O>& Val) const
		{
			return Vec3(*this) |= Val;
		}

		template<typename O>
		Vec3& operator^=(const O& Val)
		{
			X ^= Val;
			Y ^= Val;
			Z ^= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator^=(const Vec3<O>& Val)
		{
			X = Y * Val.Z - Z * Val.Y;
			Y = Z * Val.X - X * Val.Z;
			Z = X * Val.Y - Y * Val.X;
			return *this;
		}

		template<typename O>
		Vec3 operator^(const O& Val) const
		{
			return Vec3(*this) ^= Val;
		}

		template<typename O>
		Vec3 operator^(const Vec3<O>& Val) const
		{
			return Vec3(*this) ^= Val;
		}

		template<typename O>
		Vec3& operator<<=(const O& Val)
		{
			X <<= Val;
			Y <<= Val;
			Z <<= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator<<=(const Vec3<O>& Val)
		{
			X <<= Val.X;
			Y <<= Val.Y;
			Z <<= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator<<(const O& Val) const
		{
			return Vec3(*this) <<= Val;
		}

		template<typename O>
		Vec3 operator<<(const Vec3<O>& Val) const
		{
			return Vec3(*this) <<= Val;
		}

		template<typename O>
		Vec3& operator>>=(const O& Val)
		{
			X >>= Val;
			Y >>= Val;
			Z >>= Val;
			return *this;
		}

		template<typename O>
		Vec3& operator>>=(const Vec3<O>& Val)
		{
			X >>= Val.X;
			Y >>= Val.Y;
			Z >>= Val.Z;
			return *this;
		}

		template<typename O>
		Vec3 operator>>(const O& Val) const
		{
			return Vec3(*this) >>= Val;
		}

		template<typename O>
		Vec3 operator>>(const Vec3<O>& Val) const
		{
			return Vec3(*this) >>= Val;
		}

		Vec3 operator~() const
		{
			return Vec3(~X, ~Y, ~Z);
		}

		Vec3& operator++()
		{
			X++;
			Y++;
			Z++;
			return *this;
		}

		Vec3& operator++(int)
		{
			++X;
			++Y;
			++Z;
			return *this;
		}

		Vec3& operator--()
		{
			X--;
			Y--;
			Z--;
			return *this;
		}

		Vec3& operator--(int)
		{
			--X;
			--Y;
			--Z;
			return *this;
		}

		template<typename O>
		bool operator==(const Vec3<O>& Val) const
		{
			return X == Val.X && Y == Val.Y && Z == Val.Z;
		}

		template<typename O>
		bool operator!=(const Vec3<O>& Val) const
		{
			return !(*this == Val);
		}

	public:
		T X = 0;
		T Y = 0;
		T Z = 0;
	};

	typedef Vec3<float> Vec3f;
	typedef Vec3<int> Vec3i;
	typedef Vec3<unsigned int> Vec3ui;
}
