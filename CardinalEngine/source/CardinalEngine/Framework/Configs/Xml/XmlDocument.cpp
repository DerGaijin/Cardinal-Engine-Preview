#include "CardinalEngine/Framework/Configs/Xml/XmlDocument.h"

namespace CE
{
	XmlElement& XmlDocument::AddElement(const XmlElement& Element)
	{
		return m_Elements.EmplaceRef(Element);
	}

	CE::String XmlDocument::Write(const String& Whitespace /*= "\t"*/) const
	{
		String Result;
		for (auto& Element : m_Elements)
		{
			Element.Write(Result, Whitespace, 0);
		}
		return Result;
	}

	bool XmlDocument::HasElement(const String& Element) const
	{
		size_t FoundAt = 0;
		return m_Elements.FindByPredicate([&](const XmlElement& It) { return It.Text() == Element; }, FoundAt);
	}

	const CE::XmlElement& XmlDocument::GetElement(const String& Element) const
	{
		static XmlElement Empty(true, "");
		size_t FoundAt = 0;
		if (m_Elements.FindByPredicate([&](const XmlElement& It) { return It.Text() == Element; }, FoundAt))
		{
			return m_Elements[FoundAt];
		}
		return Empty;
	}

	void XmlElement::SetText(const String& Text)
	{
		m_Text = Text;
	}

	XmlElement& XmlElement::AddSubElement(const XmlElement& Element)
	{
		return m_SubElements.EmplaceRef(Element);
	}

	void XmlElement::SetAttribute(const String& Attribute, const String& Value)
	{
		size_t FoundAt = 0;
		if (m_Attributes.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Attribute; }, FoundAt))
		{
			m_Attributes[FoundAt].second = Value;
		}
		else
		{
			m_Attributes.Emplace(Attribute, Value);
		}
	}

	void XmlElement::Write(String& Result, const String& WhiteSpace, size_t Depth) const
	{
		Result.Reserve(Result.Size());
		auto PrintWhitespace = [&](size_t Depth)
		{
			for (size_t i = 0; i < Depth; i++)
			{
				Result.Append(WhiteSpace);
			}
		};

		PrintWhitespace(Depth);
		if (m_IsText)
		{
			Result.Append(m_Text);
		}
		else
		{
			Result.Append("<");
			if (m_IsInstruction)
			{
				Result.Append("?");
			}
			Result.Append(m_Text);

			for (auto& Attribute : m_Attributes)
			{
				Result.Append(" ");
				Result.Append(Attribute.first);
				Result.Append("=\"");
				Result.Append(Attribute.second);
				Result.Append("\"");
			}

			if (m_IsInstruction)
			{
				Result.Append("?");
			}
			Result.Append(">");

			if (!m_IsInstruction)
			{
				if (m_SubElements.Size() == 1 && m_SubElements[m_SubElements.Size() - 1].m_IsText && m_SubElements[m_SubElements.Size() - 1].m_Text.Size() < 40)
				{
					Result.Append(m_SubElements[m_SubElements.Size() - 1].m_Text);
				}
				else if (m_SubElements.Size() > 0)
				{
					Result.Append("\n");

					for (auto& SubElement : m_SubElements)
					{
						SubElement.Write(Result, WhiteSpace, Depth + 1);
						if (SubElement.m_IsText)
						{
							Result.Append("\n");
						}
					}

					PrintWhitespace(Depth);
				}

				Result.Append("</");
				Result.Append(m_Text);
				Result.Append(">\n");
			}
			else
			{
				Result.Append("\n");
			}
		}
	}

	bool XmlElement::HasAttribute(const String& Attribute) const
	{
		size_t FoundAt = 0;
		return m_Attributes.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Attribute; }, FoundAt);
	}

	const CE::String& XmlElement::GetAttribute(const String& Attribute) const
	{
		static String Empty;
		size_t FoundAt = 0;
		if (m_Attributes.FindByPredicate([&](const std::pair<String, String>& It) { return It.first == Attribute; }, FoundAt))
		{
			return m_Attributes[FoundAt].second;
		}
		return Empty;
	}

	bool XmlElement::HasElement(const String& Element) const
	{
		size_t FoundAt = 0;
		return m_SubElements.FindByPredicate([&](const XmlElement& It) { return It.Text() == Element; }, FoundAt);
	}

	const CE::XmlElement& XmlElement::GetElement(const String& Element) const
	{
		static XmlElement Empty(true, "");
		size_t FoundAt = 0;
		if (m_SubElements.FindByPredicate([&](const XmlElement& It) { return It.Text() == Element; }, FoundAt))
		{
			return m_SubElements[FoundAt];
		}
		return Empty;
	}
}
