#pragma once
#include "../../Core/Headers.h"

namespace CE
{
	class CE_API XmlElement
	{
	public:
		// Constructor
		XmlElement(bool IsText, const String& Text, bool IsInstruction = false) : m_IsText(IsText), m_Text(Text), m_IsInstruction(IsInstruction) {}

		// Sets the Text for this Element
		void SetText(const String& Text);

		// Adds a new Element as a Sub Element
		XmlElement& AddSubElement(const XmlElement& Element);

		// Sets the Attributes Value
		void SetAttribute(const String& Attribute, const String& Value);

	public:
		// Prints the Element to String
		void Write(String& Result, const String& WhiteSpace, size_t Depth) const;

		// Returns true if the Element is a Text Element
		bool IsText() const
		{
			return m_IsText;
		}

		// Returns true if the Element is a Instruction
		bool IsInstruction() const
		{
			return m_IsInstruction;
		}

		// Returns the Text
		const String& Text() const
		{
			return m_Text;
		}

		// Returns all SubElements
		const Array<XmlElement>& SubElements() const
		{
			return m_SubElements;
		}

		// Returns all Attributes and its Values
		const Array<std::pair<String, String>>& Attributes() const
		{
			return m_Attributes;
		}

		// Returns true if the Element contains the Attribute
		bool HasAttribute(const String& Attribute) const;

		// Returns the Value of a Attribute
		const String& GetAttribute(const String& Attribute) const;

		// Returns true if the Element contains the Sub Element
		bool HasElement(const String& Element) const;

		// Returns the Sub Element
		const XmlElement& GetElement(const String& Element) const;

	private:
		const bool m_IsText;
		const bool m_IsInstruction;
		String m_Text;
		Array<XmlElement> m_SubElements;
		Array<std::pair<String, String>> m_Attributes;
	};

	class CE_API XmlDocument
	{
	public:
		// Adds a Element
		XmlElement& AddElement(const XmlElement& Element);

	public:
		// Returns the Xml Document as a String
		String Write(const String& Whitespace = "\t") const;

		// Returns true if the Document contains the Element
		bool HasElement(const String& Element) const;

		// Returns the Element
		const XmlElement& GetElement(const String& Element) const;

		// Returns all Elements
		const Array<XmlElement>& Elements() const
		{
			return m_Elements;
		}

	private:
		Array<XmlElement> m_Elements;
	};
}
