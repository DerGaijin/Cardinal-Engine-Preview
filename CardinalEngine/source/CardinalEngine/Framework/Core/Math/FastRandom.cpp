#include "CardinalEngine/Framework/Core/Math/FastRandom.h"

namespace CE
{
	FastRandom::FastRandom(int Seed) : m_State(Seed)
	{

	}

	void FastRandom::SetSeed(int Seed)
	{
		m_State = Seed;
	}

	int32 FastRandom::GetInt32() noexcept
	{
		m_State = (LCG_A * m_State + LCG_C) % LCG_M;
		return m_State;
	}

	float FastRandom::GetFloat32InRange(float low, float high) noexcept
	{
		const auto scaleToRangeF32 = [](float x, float low, float high)
		{
			return low + x * (high - low);
		};
		return scaleToRangeF32(GetFloat32(), low, high);
	}

	double FastRandom::GetFloat64InRange(double low, double high) noexcept
	{
		const auto scaleToRangeF64 = [](double x, double low, double high)
		{
			return low + x * (high - low);
		};
		return scaleToRangeF64(GetFloat64(), low, high);
	}

	int32 FastRandom::GetInt32InRange(int low, int high) noexcept
	{
		if (low >= high)
			return low;

		return low + GetUInt32() / (0xFFFFFFFF / (high - low + 1) + 1);
	}
}
