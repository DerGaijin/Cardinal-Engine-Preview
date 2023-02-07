#include "CardinalEngine/Framework/Core/Math/Color.h"

#include <math.h>
#include <utility>

namespace CE
{
	Color Color::FromRGB(float R, float G, float B)
	{
		return Color{ R, G, B, 1.0f };
	}

	Color Color::FromRGBA(float R, float G, float B, float A)
	{
		return Color{ R, G, B, A };
	}

	Color Color::FromHSV(float H, float S, float V)
	{
		return FromHSVA(H, S, V, 1.0f);
	}

	Color Color::FromHSVA(float H, float S, float V, float A)
	{
		Color Result;
		Result.A = A;
		if (S == 0.0f)
		{
			Result.R = Result.G = Result.B = V;
			return Result;
		}

		H = fmodf(H, 1.0f) / (60.0f / 360.0f);
		int   i = (int)H;
		float f = H - (float)i;
		float p = V * (1.0f - S);
		float q = V * (1.0f - S * f);
		float t = V * (1.0f - S * (1.0f - f));

		switch (i)
		{
		case 0: Result.R = V; Result.G = t; Result.B = p; break;
		case 1: Result.R = q; Result.G = V; Result.B = p; break;
		case 2: Result.R = p; Result.G = V; Result.B = t; break;
		case 3: Result.R = p; Result.G = q; Result.B = V; break;
		case 4: Result.R = t; Result.G = p; Result.B = V; break;
		case 5: default: Result.R = V; Result.G = p; Result.B = q; break;
		}
		return Result;
	}

	Color Color::FromIntegrals(uint8 R, uint8 G, uint8 B, uint8 A)
	{
		return Color(R / 255.0f, R / 255.0f, R / 255.0f, R / 255.0f);
	}

	Color::Color(int32 Hex)
	{
		R = ((Hex & 0xff000000) >> 24) / 255.0f;
		G = ((Hex & 0x00ff0000) >> 16) / 255.0f;
		B = ((Hex & 0x0000ff00) >> 8) / 255.0f;
		A = ((Hex & 0x000000ff) >> 0) / 255.0f;
	}

	Vec3f Color::ToHSV() const
	{
		Vec3f Result;

		float TempR = R;
		float TempG = G;
		float TempB = B;
		float K = 0.f;
		if (TempG < TempB)
		{
			std::swap(TempG, TempB);
			K = -1.f;
		}
		if (TempR < TempG)
		{
			std::swap(TempR, TempG);
			K = -2.f / 6.f - K;
		}

		const float chroma = TempR - (TempG < TempB ? TempG : TempB);
		Result.X = fabsf(K + (TempG - TempB) / (6.f * chroma + 1e-20f));
		Result.Y = chroma / (TempR + 1e-20f);
		Result.Z = TempR;

		return Result;
	}

	Vec4f Color::ToHSVA() const
	{
		return Vec4f(ToHSV(), 1.0f);
	}
}
