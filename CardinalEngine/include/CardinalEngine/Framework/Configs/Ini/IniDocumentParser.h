#pragma once
#include "../../Core/Headers.h"
#include "IniDocument.h"
#include "IniParser.h"

namespace CE
{
	class CE_API IniDocumentParser : public IniParser
	{
	public:
		// Constructor with Document
		IniDocumentParser(IniDocument& Document) : m_Document(Document) {}

	protected:
		virtual void OnParsed_Section(const String& Section) override;
		virtual void OnParsed_KeyValue(const String& Key, const String& Value) override;
		virtual void OnParsed_Comment(const String& Comment) override;

	private:
		IniDocument& m_Document;
		IniSection* CurrSection = nullptr;
	};
}
