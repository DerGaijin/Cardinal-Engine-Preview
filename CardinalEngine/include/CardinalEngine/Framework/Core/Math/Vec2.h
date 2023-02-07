#pragma once
#include "../Definitions.h"
#include "../Logging/Log.h"
#include <type_traits>

namespace CE
{
	template<typename T>
	class Vec2
	{
		static_assert(std::is_arithmetic_v<T>, "T has to be Arithmetic");
	public:
		// Constructor
		Vec2() = default;

		// Copy Constructor
		Vec2(const Vec2& Other) = default;

		// Move Constructor
		Vec2(Vec2&& Other) = default;

		// Constructor with Single Value
		Vec2(const T& Val) : X(Val), Y(Val) {}

		// Constructor with X and Y
		Vec2(const T& X, const T& Y) : X(X), Y(Y) {}


	public:
		// Returns the Distance to the Other Vec
		float Distance(const Vec2& Other) const
		{
			return Vec2(Other - *this).Length();
		}

		// Normalizes the Vec
		Vec2 Normalize() const
		{
			return *this * (1 / std::sqrt(Dot(*this)));
		}

		// Returns the Dot product with the Other Vec
		float Dot(const Vec2& Other) const
		{
			Vec2 Tmp(*this * Other);
			return Tmp.X + Tmp.Y;
		}

		// Returns the Length of the Vec
		float Length() const
		{
			return std::sqrt(Dot(*this));
		}

	public:
		const T& operator[](int Comp) const
		{
			CE_CHECK(Comp >= 0 && Comp <= 1);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			}
		}

		T& operator[](int Comp)
		{
			CE_CHECK(Comp >= 0 && Comp <= 1);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			}
		}

		template<typename O>
		Vec2& operator=(const O& Val)
		{
			X = Y = Val;
			return *this;
		}

		Vec2& operator=(const Vec2&) = default;

		Vec2& operator=(Vec2&&) = default;

		Vec2 operator+() const
		{
			return *this;
		}

		Vec2 operator-() const
		{
			return Vec2(-X, -Y);
		}

		template<typename O>
		Vec2& operator+=(const O& Val)
		{
			X += Val;
			Y += Val;
			return *this;
		}

		template<typename O>
		Vec2& operator+=(const Vec2<O>& Val)
		{
			X += Val.X;
			Y += Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator+(const O& Val) const
		{
			return Vec2(*this) += Val;
		}

		template<typename O>
		Vec2 operator+(const Vec2<O>& Val) const
		{
			return Vec2(*this) += Val;
		}

		template<typename O>
		Vec2& operator-=(const O& Val)
		{
			X -= Val;
			Y -= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator-=(const Vec2<O>& Val)
		{
			X -= Val.X;
			Y -= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator-(const O& Val) const
		{
			return Vec2(*this) -= Val;
		}

		template<typename O>
		Vec2 operator-(const Vec2<O>& Val) const
		{
			return Vec2(*this) -= Val;
		}

		template<typename O>
		Vec2& operator*=(const O& Val)
		{
			X *= Val;
			Y *= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator*=(const Vec2<O>& Val)
		{
			X *= Val.X;
			Y *= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator*(const O& Val) const
		{
			return Vec2(*this) *= Val;
		}

		template<typename O>
		Vec2 operator*(const Vec2<O>& Val) const
		{
			return Vec2(*this) *= Val;
		}

		template<typename O>
		Vec2& operator/=(const O& Val)
		{
			X /= Val;
			Y /= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator/=(const Vec2<O>& Val)
		{
			X /= Val.X;
			Y /= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator/(const O& Val) const
		{
			return Vec2(*this) /= Val;
		}

		template<typename O>
		Vec2 operator/(const Vec2<O>& Val) const
		{
			return Vec2(*this) /= Val;
		}

		template<typename O>
		Vec2& operator%=(const O& Val)
		{
			X %= Val;
			Y %= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator%=(const Vec2<O>& Val)
		{
			X %= Val.X;
			Y %= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator%(const O& Val) const
		{
			return Vec2(*this) %= Val;
		}

		template<typename O>
		Vec2 operator%(const Vec2<O>& Val) const
		{
			return Vec2(*this) %= Val;
		}

		template<typename O>
		Vec2& operator&=(const O& Val)
		{
			X &= Val;
			Y &= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator&=(const Vec2<O>& Val)
		{
			X &= Val.X;
			Y &= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator&(const O& Val) const
		{
			return Vec2(*this) &= Val;
		}

		template<typename O>
		Vec2 operator&(const Vec2<O>& Val) const
		{
			return Vec2(*this) &= Val;
		}

		template<typename O>
		Vec2& operator|=(const O& Val)
		{
			X |= Val;
			Y |= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator|=(const Vec2<O>& Val)
		{
			X |= Val.X;
			Y |= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator|(const O& Val) const
		{
			return Vec2(*this) |= Val;
		}

		template<typename O>
		Vec2 operator|(const Vec2<O>& Val) const
		{
			return Vec2(*this) |= Val;
		}

		template<typename O>
		Vec2& operator^=(const O& Val)
		{
			X ^= Val;
			Y ^= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator^=(const Vec2<O>& Val)
		{
			X ^= Val.X;
			Y ^= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator^(const O& Val) const
		{
			return Vec2(*this) ^= Val;
		}

		template<typename O>
		Vec2 operator^(const Vec2<O>& Val) const
		{
			return Vec2(*this) ^= Val;
		}

		template<typename O>
		Vec2& operator<<=(const O& Val)
		{
			X <<= Val;
			Y <<= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator<<=(const Vec2<O>& Val)
		{
			X <<= Val.X;
			Y <<= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator<<(const O& Val) const
		{
			return Vec2(*this) <<= Val;
		}

		template<typename O>
		Vec2 operator<<(const Vec2<O>& Val) const
		{
			return Vec2(*this) <<= Val;
		}

		template<typename O>
		Vec2& operator>>=(const O& Val)
		{
			X >>= Val;
			Y >>= Val;
			return *this;
		}

		template<typename O>
		Vec2& operator>>=(const Vec2<O>& Val)
		{
			X >>= Val.X;
			Y >>= Val.Y;
			return *this;
		}

		template<typename O>
		Vec2 operator>>(const O& Val) const
		{
			return Vec2(*this) >>= Val;
		}

		template<typename O>
		Vec2 operator>>(const Vec2<O>& Val) const
		{
			return Vec2(*this) >>= Val;
		}

		Vec2 operator~() const
		{
			return Vec2(~X, ~Y, ~Z);
		}

		template<typename O>
		bool operator==(const Vec2<O>& Val) const
		{
			return X == Val.X && Y == Val.Y;
		}

		template<typename O>
		bool operator!=(const Vec2<O>& Val) const
		{
			return !(*this == Val);
		}


	public:
		T X = 0;
		T Y = 0;
	};

	typedef Vec2<float> Vec2f;
	typedef Vec2<int> Vec2i;
	typedef Vec2<unsigned int> Vec2ui;
}
