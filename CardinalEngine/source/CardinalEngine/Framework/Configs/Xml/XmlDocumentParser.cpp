#include "CardinalEngine/Framework/Configs/Xml/XmlDocumentParser.h"

namespace CE
{
	void XmlDocumentParser::OnParsed_BeginElement_Start(const String& Element, bool IsInstruction)
	{
		if (m_ElementStack.Size() > 0)
		{
			XmlElement* Last = m_ElementStack[m_ElementStack.Size() - 1];
			m_ElementStack.Emplace(&Last->AddSubElement(XmlElement(false, Element, IsInstruction)));
		}
		else
		{
			m_ElementStack.Emplace(&m_Document.AddElement(XmlElement(false, Element, IsInstruction)));
		}
		m_AcceptAttribute = true;
	}

	void XmlDocumentParser::OnParsed_BeginElement_End()
	{
		m_AcceptAttribute = false;
		if (m_ElementStack.Size() > 0 && m_ElementStack[m_ElementStack.Size() - 1]->IsInstruction())
		{
			m_ElementStack.RemoveAt(m_ElementStack.Size() - 1);
		}
	}

	void XmlDocumentParser::OnParsed_Attribute(const String& Attribute, const String& Value)
	{
		if (!m_AcceptAttribute || m_ElementStack.Size() == 0)
		{
			return;
		}

		XmlElement* Last = m_ElementStack[m_ElementStack.Size() - 1];
		Last->SetAttribute(Attribute, Value);
	}

	void XmlDocumentParser::OnParsed_Text(const String& Text)
	{
		if (m_ElementStack.Size() > 0)
		{
			XmlElement* Last = m_ElementStack[m_ElementStack.Size() - 1];
			Last->AddSubElement(XmlElement(true, Text));
		}
		else
		{
			m_Document.AddElement(XmlElement(true, Text));
		}
	}

	void XmlDocumentParser::OnParsed_EndElement(const String& Element)
	{
		if (m_ElementStack.Size() > 0)
		{
			m_ElementStack.RemoveAt(m_ElementStack.Size() - 1);
		}
	}
}
