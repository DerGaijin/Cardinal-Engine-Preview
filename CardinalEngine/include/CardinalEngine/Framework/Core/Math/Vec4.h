#pragma once
#include "../Definitions.h"
#include "../Logging/Log.h"
#include "Vec3.h"

namespace CE
{
	template<typename T>
	class Vec4
	{
		static_assert(std::is_arithmetic_v<T>, "T has to be Arithmetic");
	public:
		// Constructor
		Vec4() = default;

		// Copy Constructor
		Vec4(const Vec4&) = default;

		// Move Constructor
		Vec4(Vec4&&) = default;

		// Constructor with Single Value
		Vec4(const T& Val) : X(Val), Y(Val), Z(Val), W(Val) {}

		// Constructor with X,Y,Z and W
		Vec4(const T& X, const T& Y, const T& Z, const T& W) : X(X), Y(Y), Z(Z), W(W) {}

		// Constructor with XY and ZW
		Vec4(const Vec2<T>& XY, const Vec2<T>& ZW) : X(XY.X), Y(XY.Y), Z(ZW.X), W(ZW.Y) {}

		// Constructor with XY, Z and W
		Vec4(const Vec2<T>& XY, const T& Z, const T& W) : X(XY.X), Y(XY.Y), Z(Z), W(W) {}

		// Constructor with X, YZ and W
		Vec4(const T& X, const Vec2<T>& YZ, const T& W) : X(X), Y(YZ.X), Z(YZ.Y), W(W) {}

		// Constructor with X, Y and ZW
		Vec4(const T& X, const T& Y, const Vec2<T>& ZW) : X(X), Y(Y), Z(ZW.X), W(ZW.Y) {}

		// Constructor with XYZ and W
		Vec4(const Vec3<T> XYZ, const T& W) : X(XYZ.X), Y(XYZ.Y), Z(XYZ.Z), W(W) {}

		// Constructor with X and YZW
		Vec4(const T& X, const Vec3<T> YZW) : X(X), Y(YZW.X), Z(YZW.Y), W(YZW.Z) {}

	public:
		// Returns the Distance with the Other Vec
		float Distance(const Vec4& Other) const
		{
			return Vec4(Other - *this).Length();
		}

		// Normalizes the Vec
		Vec4 Normalize() const
		{
			return *this * (1 / std::sqrt(Dot(*this)));
		}

		// Returns the Dot product with the Other Vec
		float Dot(const Vec4& Other) const
		{
			Vec4 Tmp(*this * Other);
			return Tmp.X + Tmp.Y + Tmp.Z + Tmp.W;
		}

		// Returns the Length of the Vec
		float Length() const
		{
			return std::sqrt(Dot(*this));
		}

	public:
		const T& operator[](int Comp) const
		{
			CE_CHECK(Comp >= 0 && Comp <= 3);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
		}

		T& operator[](int Comp)
		{
			CE_CHECK(Comp >= 0 && Comp <= 3);
			switch (Comp)
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
		}

		template<typename O>
		Vec4& operator=(const O& Val)
		{
			X = Y = Z = W = Val;
			return *this;
		}

		Vec4& operator=(const Vec4&) = default;

		Vec4& operator=(Vec4&&) = default;

		Vec4 operator+() const
		{
			return *this;
		}

		Vec4 operator-() const
		{
			return Vec4(-X, -Y, -Z, -W);
		}

		template<typename O>
		Vec4& operator+=(const O& Val)
		{
			X += Val;
			Y += Val;
			Z += Val;
			W += Val;
			return *this;
		}

		template<typename O>
		Vec4& operator+=(const Vec4<O>& Val)
		{
			X += Val.X;
			Y += Val.Y;
			Z += Val.Z;
			W += Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator+(const O& Val) const
		{
			return Vec4(*this) += Val;
		}

		template<typename O>
		Vec4 operator+(const Vec4<O>& Val) const
		{
			return Vec4(*this) += Val;
		}

		template<typename O>
		Vec4& operator-=(const O& Val)
		{
			X -= Val;
			Y -= Val;
			Z -= Val;
			W -= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator-=(const Vec4<O>& Val)
		{
			X -= Val.X;
			Y -= Val.Y;
			Z -= Val.Z;
			W -= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator-(const O& Val) const
		{
			return Vec4(*this) -= Val;
		}

		template<typename O>
		Vec4 operator-(const Vec4<O>& Val) const
		{
			return Vec4(*this) -= Val;
		}

		template<typename O>
		Vec4& operator*=(const O& Val)
		{
			X *= Val;
			Y *= Val;
			Z *= Val;
			W *= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator*=(const Vec4<O>& Val)
		{
			X *= Val.X;
			Y *= Val.Y;
			Z *= Val.Z;
			W *= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator*(const O& Val) const
		{
			return Vec4(*this) *= Val;
		}

		template<typename O>
		Vec4 operator*(const Vec4<O>& Val) const
		{
			return Vec4(*this) *= Val;
		}

		template<typename O>
		Vec4& operator/=(const O& Val)
		{
			X /= Val;
			Y /= Val;
			Z /= Val;
			W /= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator/=(const Vec4<O>& Val)
		{
			X /= Val.X;
			Y /= Val.Y;
			Z /= Val.Z;
			W /= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator/(const O& Val) const
		{
			return Vec4(*this) /= Val;
		}

		template<typename O>
		Vec4 operator/(const Vec4<O>& Val) const
		{
			return Vec4(*this) /= Val;
		}

		template<typename O>
		Vec4& operator%=(const O& Val)
		{
			X %= Val;
			Y %= Val;
			Z %= Val;
			W %= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator%=(const Vec4<O>& Val)
		{
			X %= Val.X;
			Y %= Val.Y;
			Z %= Val.Z;
			W %= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator%(const O& Val) const
		{
			return Vec4(*this) %= Val;
		}

		template<typename O>
		Vec4 operator%(const Vec4<O>& Val) const
		{
			return Vec4(*this) %= Val;
		}

		template<typename O>
		Vec4& operator&=(const O& Val)
		{
			X &= Val;
			Y &= Val;
			Z &= Val;
			W &= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator&=(const Vec4<O>& Val)
		{
			X &= Val.X;
			Y &= Val.Y;
			Z &= Val.Z;
			W &= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator&(const O& Val) const
		{
			return Vec4(*this) &= Val;
		}

		template<typename O>
		Vec4 operator&(const Vec4<O>& Val) const
		{
			return Vec4(*this) &= Val;
		}

		template<typename O>
		Vec4& operator|=(const O& Val)
		{
			X |= Val;
			Y |= Val;
			Z |= Val;
			W |= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator|=(const Vec4<O>& Val)
		{
			X |= Val.X;
			Y |= Val.Y;
			Z |= Val.Z;
			W |= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator|(const O& Val) const
		{
			return Vec4(*this) |= Val;
		}

		template<typename O>
		Vec4 operator|(const Vec4<O>& Val) const
		{
			return Vec4(*this) |= Val;
		}

		template<typename O>
		Vec4& operator^=(const O& Val)
		{
			X ^= Val;
			Y ^= Val;
			Z ^= Val;
			W ^= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator^=(const Vec4<O>& Val)
		{
			X ^= Val.X;
			Y ^= Val.Y;
			Z ^= Val.Z;
			W ^= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator^(const O& Val) const
		{
			return Vec4(*this) ^= Val;
		}

		template<typename O>
		Vec4 operator^(const Vec4<O>& Val) const
		{
			return Vec4(*this) ^= Val;
		}

		template<typename O>
		Vec4& operator<<=(const O& Val)
		{
			X <<= Val;
			Y <<= Val;
			Z <<= Val;
			W <<= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator<<=(const Vec4<O>& Val)
		{
			X <<= Val.X;
			Y <<= Val.Y;
			Z <<= Val.Z;
			W <<= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator<<(const O& Val) const
		{
			return Vec4(*this) <<= Val;
		}

		template<typename O>
		Vec4 operator<<(const Vec4<O>& Val) const
		{
			return Vec4(*this) <<= Val;
		}

		template<typename O>
		Vec4& operator>>=(const O& Val)
		{
			X >>= Val;
			Y >>= Val;
			Z >>= Val;
			W >>= Val;
			return *this;
		}

		template<typename O>
		Vec4& operator>>=(const Vec4<O>& Val)
		{
			X >>= Val.X;
			Y >>= Val.Y;
			Z >>= Val.Z;
			W >>= Val.W;
			return *this;
		}

		template<typename O>
		Vec4 operator>>(const O& Val) const
		{
			return Vec4(*this) >>= Val;
		}

		template<typename O>
		Vec4 operator>>(const Vec4<O>& Val) const
		{
			return Vec4(*this) >>= Val;
		}

		Vec4 operator~() const
		{
			return Vec4(~X, ~Y, ~Z);
		}

		template<typename O>
		bool operator==(const Vec4<O>& Val) const
		{
			return X == Val.X && Y == Val.Y && Z == Val.Z && W == Val.W;
		}

		template<typename O>
		bool operator!=(const Vec4<O>& Val) const
		{
			return !(*this == Val);
		}

	public:
		T X = 0;
		T Y = 0;
		T Z = 0;
		T W = 0;
	};

	typedef Vec4<float> Vec4f;
	typedef Vec4<int> Vec4i;
	typedef Vec4<unsigned int> Vec4ui;
}
