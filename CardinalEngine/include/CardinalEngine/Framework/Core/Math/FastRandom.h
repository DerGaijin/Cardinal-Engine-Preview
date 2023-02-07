#pragma once
#include "../Definitions.h"
#include "Fundamentals.h"

namespace CE
{
	class CE_API FastRandom
	{
	private:
		static constexpr auto DEFAULT_LCG_SEED = 4321;
		static constexpr auto LCG_M = 2147483647;
		static constexpr auto LCG_A = 48271;
		static constexpr auto LCG_C = 0;

	public:
		FastRandom() = default;

		FastRandom(int Seed);

		void SetSeed(int Seed);

	public:
		int GetSeed() const noexcept
		{
			return m_State;
		}

		int32 GetInt32() noexcept;

		inline uint32 GetUInt32() noexcept { return (unsigned int)GetInt32(); }
		
		inline int16 GetInt16() noexcept { return (short)(GetInt32() & 0xFFFF); }
		
		inline uint16 GetUInt16() noexcept { return (unsigned short)(GetInt16() & 0xFFFF); }
		
		inline double GetFloat64() noexcept { return GetInt32() / (double)0x7FFFFFFF; }
		
		inline float GetFloat32() noexcept { return (float)GetFloat64(); }


		float GetFloat32InRange(float low, float high) noexcept;

		double GetFloat64InRange(double low, double high) noexcept;

		int32 GetInt32InRange(int low, int high) noexcept;

		inline float GetInRange(float low, float high) { return GetFloat32InRange(low, high); }

		inline double GetInRange(double low, double high) { return GetFloat64InRange(low, high); }
		
		inline int32 GetInRange(int low, int high) { return GetInt32InRange(low, high); }

	private:
		int m_State = DEFAULT_LCG_SEED;
	};
}
