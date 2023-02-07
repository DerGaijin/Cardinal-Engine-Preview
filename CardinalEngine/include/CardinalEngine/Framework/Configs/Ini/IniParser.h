#pragma once
#include "../../Core/Headers.h"
#include "../ConfigParser.h"

namespace CE
{
	class CE_API IniParser : public ConfigParser
	{
	public:
		// Parses the Ini Config
		bool Parse(ConfigParseException* Error = nullptr);

	private:
		void Process_Comment(ConfigParseToken& Token);
		void Process_Section(ConfigParseToken& Token);
		void Process_KeyValue(ConfigParseToken& Token);

	protected:
		// Called when a new Section Begins
		virtual void OnParsed_Section(const String& Section) = 0;

		// Called when a Key Value pair was found
		virtual void OnParsed_KeyValue(const String& Key, const String& Value) = 0;

		// Called when a Comment was found
		virtual void OnParsed_Comment(const String& Comment) = 0;
	};
}
