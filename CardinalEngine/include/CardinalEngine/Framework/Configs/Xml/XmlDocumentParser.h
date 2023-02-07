#pragma once
#include "../../Core/Headers.h"
#include "XmlDocument.h"
#include "XmlParser.h"

namespace CE
{
	class CE_API XmlDocumentParser : public XmlParser
	{
	public:
		XmlDocumentParser(XmlDocument& Document) : m_Document(Document) {}

	protected:
		virtual void OnParsed_BeginElement_Start(const String& Element, bool IsInstruction) override;
		virtual void OnParsed_BeginElement_End() override;
		virtual void OnParsed_Attribute(const String& Attribute, const String& Value) override;
		virtual void OnParsed_Text(const String& Text) override;
		virtual void OnParsed_EndElement(const String& Element) override;

	private:
		XmlDocument& m_Document;
		Array<XmlElement*> m_ElementStack;
		bool m_AcceptAttribute = false;
	};
}
