#include "CardinalEngine/Framework/Configs/Ini/IniDocument.h"

namespace CE
{
	bool IniSection::HasKey(const String& Key) const
	{
		size_t FoundAt = 0;
		return m_KeyValues.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Key; }, FoundAt);
	}

	CE::String& IniSection::operator[](const String& Key)
	{
		size_t FoundAt = 0;
		if (m_KeyValues.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Key; }, FoundAt))
		{
			return m_KeyValues[FoundAt].second;
		}
		else
		{
			return m_KeyValues.EmplaceRef(Key, "").second;
		}
	}

	const CE::String& IniSection::operator[](const String& Key) const
	{
		static String Empty;
		size_t FoundAt = 0;
		if (m_KeyValues.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Key; }, FoundAt))
		{
			return m_KeyValues[FoundAt].second;
		}
		return Empty;
	}

	bool IniDocument::HasSection(const String& Section) const
	{
		size_t FoundAt = 0;
		return m_Sections.FindByPredicate([&](const IniSection& It) { return It.Name() == Section; }, FoundAt);
	}

	CE::String IniDocument::Write() const
	{
		String Result;
		bool FirstSection = true;
		for (auto& Section : m_Sections)
		{
			if (FirstSection)
			{
				FirstSection = false;
			}
			else
			{
				Result.Append("\n");
			}

			Result.Reserve(Result.Size() + Section.Name().Size() + 3);
			Result.Append("[");
			Result.Append(Section.Name());
			Result.Append("]");
			Result.Append('\n');

			for (auto& [Key, Value] : Section.KeyValues())
			{
				Result.Reserve(Result.Size() + Key.Size() + Value.Size() + 4);
				Result.Append(Key);
				Result.Append(" = ");
				Result.Append(Value);
				Result.Append('\n');
			}
		}
		return Result;
	}

	bool IniDocument::HasKey(const String& Section, const String& Key) const
	{
		if (HasSection(Section))
		{
			return (*this)[Section].HasKey(Key);
		}
		return false;
	}

	String& IniDocument::Value(const String& Section, const String& Key)
	{
		return (*this)[Section][Key];
	}

	const String& IniDocument::Value(const String& Section, const String& Key) const
	{
		return (*this)[Section][Key];
	}

	CE::IniSection& IniDocument::operator[](const String& Section)
	{
		size_t FoundAt = 0;
		if (m_Sections.FindByPredicate([&](const IniSection& It) { return It.Name() == Section; }, FoundAt))
		{
			return m_Sections[FoundAt];
		}
		else
		{
			return m_Sections.EmplaceRef(IniSection(Section));
		}
	}

	const CE::IniSection& IniDocument::operator[](const String& Section) const
	{
		static IniSection Empty("");
		size_t FoundAt = 0;
		if (m_Sections.FindByPredicate([&](const IniSection& It) { return It.Name() == Section; }, FoundAt))
		{
			return m_Sections[FoundAt];
		}
		return Empty;
	}
}
