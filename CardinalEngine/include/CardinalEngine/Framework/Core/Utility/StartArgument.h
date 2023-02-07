#pragma once
#include "../Definitions.h"
#include "../Containers/Array.h"
#include "../Containers/Map.h"
#include "../Containers/String.h"

#include <filesystem>

namespace CE
{
	class CE_API StartArgument
	{
	public:
		// Parses the Start Arguments
		static void Parse(int Argc, wchar_t** Argv);

		// Returns true if the Start Arguments contains the Flag
		static bool HasFlag(const Text& Flag);

		// Returns true if the Start Arguments contains Any of the Flags
		static bool HasAnyFlag(const Array<Text>& Flags);

		// Returns true if the Start Arguments contains the Key
		static bool HasValue(const Text& Key);

		// Returns true if the Start Arguments contains Any of the Keys
		static bool HasAnyValue(const Array<Text>& Keys);

		// Returns the Value of the Key
		static const StartArgument& GetValue(const Text& Key);

		// Returns the Value of the first found Key
		static const StartArgument& GetAnyValue(const Array<Text>& Keys);


		// Overrides a Path
		static bool OverridePath(std::filesystem::path& Path, const Text& Key, bool CheckExists = true, bool CheckIsDirectory = true);

	private:
		StartArgument(const Text& Arg) : m_Argument(Arg) {}

	public:
		// Returns true if the Start Argument is Valid
		bool IsValid() const;

		// Returns the Text of the Start Argument
		const Text& Text() const;

	private:
		static void Clear();

	private:
		static Array<CE::Text> s_Raw;
		static Array<CE::Text> s_Flags;
		static std::map<CE::Text, StartArgument> s_KeyValues;

	private:
		CE::Text m_Argument;
	};
}
