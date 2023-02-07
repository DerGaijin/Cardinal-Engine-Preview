#include "CardinalEngine/Framework/Configs/Ini/IniDocumentParser.h"

namespace CE
{
	void IniDocumentParser::OnParsed_Section(const String& Section)
	{
		CurrSection = &m_Document[Section];
	}

	void IniDocumentParser::OnParsed_KeyValue(const String& Key, const String& Value)
	{
		if (CurrSection == nullptr)
		{
			m_Document[""][Key] = Value;
		}
		else
		{
			(*CurrSection)[Key] = Value;
		}
	}

	void IniDocumentParser::OnParsed_Comment(const String& Comment)
	{

	}
}
