#include "CardinalEngine/Framework/Core/Utility/StartArgument.h"

namespace CE
{
	CE::Array<CE::Text> StartArgument::s_Raw;

	CE::Array<CE::Text> StartArgument::s_Flags;

	std::map<CE::Text, CE::StartArgument> StartArgument::s_KeyValues;

	void StartArgument::Parse(int Argc, wchar_t** Argv)
	{
		Clear();

		for (int i = 0; i < Argc; i++)
		{
			const CE::Text& Arg = s_Raw.EmplaceRef(Argv[i]);

			size_t FoundAt = Arg.Find(L"=");
			if (FoundAt != Text::NPos)
			{
				CE::Text Key = Arg.SubString(0, FoundAt);
				for (size_t i = 0; i < Key.Size(); i++)
				{
					if (Key[i] != L'-')
					{
						Key.RemoveAt(0, i);
						break;
					}
				}
				Key.ToUpper();
				Key.LTrim();
				Key.RTrim();
				CE::Text Value = Arg.SubString(FoundAt + 1);
				Value.LTrim();
				Value.RTrim();

				s_KeyValues.emplace(Key, StartArgument(Value));
			}
			else if (Arg.StartsWith(L"-"))
			{
				CE::Text& Flag = s_Flags.EmplaceRef(Arg);
				Flag.ToUpper();
				for (size_t i = 0; i < Flag.Size(); i++)
				{
					if (Flag[i] != L'-')
					{
						Flag.RemoveAt(0, i);
						break;
					}
				}
			}
		}
	}

	bool StartArgument::HasFlag(const CE::Text& Flag)
	{
		CE::Text UpperFlag = Flag;
		UpperFlag.ToUpper();
		return s_Flags.Contains(UpperFlag);
	}

	bool StartArgument::HasAnyFlag(const Array<CE::Text>& Flags)
	{
		for (auto& Flag : Flags)
		{
			if (HasFlag(Flag))
			{
				return true;
			}
		}
		return false;
	}

	bool StartArgument::HasValue(const CE::Text& Key)
	{
		CE::Text UpperKey = Key;
		UpperKey.ToUpper();
		return s_KeyValues.find(UpperKey) != s_KeyValues.end();
	}

	bool StartArgument::HasAnyValue(const Array<CE::Text>& Keys)
	{
		for (auto& Key : Keys)
		{
			if (HasValue(Key))
			{
				return true;
			}
		}
		return false;
	}

	const CE::StartArgument& StartArgument::GetValue(const CE::Text& Key)
	{
		CE::Text UpperKey = Key;
		UpperKey.ToUpper();
		auto Found = s_KeyValues.find(UpperKey);
		if (Found != s_KeyValues.end())
		{
			return Found->second;
		}
		static StartArgument Empty = CE::Text();
		return Empty;
	}

	const CE::StartArgument& StartArgument::GetAnyValue(const Array<CE::Text>& Keys)
	{
		for (auto& Key : Keys)
		{
			CE::Text UpperKey = Key;
			UpperKey.ToUpper();
			auto Found = s_KeyValues.find(UpperKey);
			if (Found != s_KeyValues.end())
			{
				return Found->second;
			}
		}
		static StartArgument Empty = CE::Text();
		return Empty;
	}

	bool StartArgument::OverridePath(std::filesystem::path& Path, const CE::Text& Key, bool CheckExists /*= true*/, bool CheckIsDirectory /*= true*/)
	{
		StartArgument Value = StartArgument::GetValue(Key);
		if (Value.IsValid())
		{
			std::filesystem::path NewPath = Value.Text().Data();
			if (CheckExists && !std::filesystem::exists(NewPath))
			{
				CE_LOG(ELog::Warning, "Failed to override Path with Startargument '%S' because the Path '%s' does not exist", Key.Data(), NewPath.u8string().c_str());
				return false;
			}
			if (CheckIsDirectory && !std::filesystem::is_directory(NewPath))
			{
				CE_LOG(ELog::Warning, "Failed to override Path with Startargument '%S' because the Path '%s' is not a Directory", Key.Data(), NewPath.u8string().c_str());
				return false;
			}
			Path = NewPath.lexically_normal();
			return true;
		}
		return false;
	}

	bool StartArgument::IsValid() const
	{
		return m_Argument.Size() > 0;
	}

	const CE::Text& StartArgument::Text() const
	{
		return m_Argument;
	}

	void StartArgument::Clear()
	{
		s_Raw.Clear();
		s_Flags.Clear();
		s_KeyValues.clear();
	}
}
