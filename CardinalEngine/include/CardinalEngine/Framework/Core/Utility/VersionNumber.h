#pragma once
#include "../Definitions.h"

namespace CE
{
	class CE_API VersionNumber
	{
	public:
		// Constructor
		VersionNumber() = default;

		// Constructor with Value
		constexpr VersionNumber(unsigned int Value) : Value(Value) {}

		// Constructor with Major,Minor,Patch
		constexpr VersionNumber(char Major, short Minor, char Patch) : Value(0), Major(Major), Minor(Minor), Patch(Patch) {}

		// Equal Operator
		inline bool operator==(const VersionNumber& Other) const
		{
			return Value == Other.Value;
		}

		// Unequal Operator
		inline bool operator!=(const VersionNumber& Other) const
		{
			return Value != Other.Value;
		}

		// Smaller Operator
		inline bool operator<(const VersionNumber& Other) const
		{
			if (Major == Other.Major)
			{
				if (Minor == Other.Minor)
				{
					return Patch < Other.Patch;
				}
				return Minor < Other.Minor;
			}
			return Major < Other.Major;
		}

	public:
		union
		{
			unsigned int Value = 0;
			struct
			{
				char Major;
				short Minor;
				char Patch;
			};
		};
	};
}
