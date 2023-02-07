#pragma once
#include "../../Core/Headers.h"

namespace CE
{
	class CE_API IniSection
	{
	public:
		// Constructor with Name
		IniSection(const String& Name) : m_Name(Name) {}

		// Returns the Name
		const String& Name() const
		{
			return m_Name;
		}

		// Returns all Keys with Values
		const Array<std::pair<String, String>>& KeyValues() const
		{
			return m_KeyValues;
		}

		// Returns true if the Section contains the Key
		bool HasKey(const String& Key) const;

	public:
		String& operator[](const String& Key);

		const String& operator[](const String& Key) const;

	private:
		String m_Name;
		Array<std::pair<String, String>> m_KeyValues;
	};


	class CE_API IniDocument
	{
	public:
		// Returns all Sections
		const Array<IniSection>& Sections() const
		{
			return m_Sections;
		}

		// Returns true if the Document contains the Section
		bool HasSection(const String& Section) const;

		// Returns the Ini as a String
		String Write() const;

		// Returns true if the Section exists and contains the Key
		bool HasKey(const String& Section, const String& Key) const;

		// Returns the Value for this Section and Key
		String& Value(const String& Section, const String& Key);

		// Returns the Value for this Section and Key
		const String& Value(const String& Section, const String& Key) const;


	public:
		IniSection& operator[](const String& Section);

		const IniSection& operator[](const String& Section) const;

	private:
		Array<IniSection> m_Sections;
	};
}
