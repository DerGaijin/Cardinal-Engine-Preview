#pragma once
#include "../../Core/Headers.h"
#include "../ConfigParser.h"

namespace CE
{
	class CE_API XmlParser : public ConfigParser
	{
	public:
		// Parses the XML Config
		bool Parse(ConfigParseException* Error = nullptr);

	private:
		void ProcessElement(ConfigParseToken& Token);
		void ProcessAttribute(ConfigParseToken& Token);
		void ProcessText(ConfigParseToken& Token);
		void ProcessInstructionEnd(ConfigParseToken& Token);

	protected:
		// Called when a new Element begins
		virtual void OnParsed_BeginElement_Start(const String& Element, bool IsInstruction) = 0;

		// Called when a Element Begin ends
		virtual void OnParsed_BeginElement_End() = 0;

		// Called when a Attribute was found
		virtual void OnParsed_Attribute(const String& Attribute, const String& Value) = 0;

		// Called when Text was found
		virtual void OnParsed_Text(const String& Text) = 0;

		// Called when a Element ends
		virtual void OnParsed_EndElement(const String& Element) = 0;

	private:
		bool m_InElementBegin = false;
		bool m_InProcessorInstruction = false;
	};
}
