#include "CardinalEngine/Framework/Configs/Ini/IniParser.h"

namespace CE
{
	bool IniParser::Parse(ConfigParseException* Error /*= nullptr*/)
	{
		try
		{
			ConfigParseToken Token;
			while (GetToken(Token, true))
			{
				if (Token.Type == ConfigParseToken::EType::Symbol && (Token.Equals(";") || Token.Equals("#")))
				{
					Process_Comment(Token);
				}
				else if (Token.Type == ConfigParseToken::EType::Symbol && Token.Equals("["))
				{
					Process_Section(Token);
				}
				else
				{
					Process_KeyValue(Token);
				}
			}
		}
		catch (const ConfigParseException& Err)
		{
			if (Error != nullptr)
			{
				*Error = Err;
			}
			return false;
		}
		return true;
	}

	void IniParser::Process_Comment(ConfigParseToken& Token)
	{
		String Comment;
		char C = GetChar();
		while (!IsEOL(C))
		{
			Comment.Append(C);
			C = GetChar();
		}

		OnParsed_Comment(Comment);
	}

	void IniParser::Process_Section(ConfigParseToken& Token)
	{
		String Section;
		char C = GetChar();
		while (!IsEOL(C) && C != ']')
		{
			Section.Append(C);
			C = GetChar();
		}

		if (C != ']')
		{
			throw ConfigParseException("Expected Section closing Bracket ']'", Token.Line, Token.Pos, Token.Text.Size());
		}

		OnParsed_Section(Section);
	}

	void IniParser::Process_KeyValue(ConfigParseToken& Token)
	{
		String Key = Token.Text;
		char C = GetChar();
		while (!IsEOL(C) && C != '=')
		{
			Key.Append(C);
			C = GetChar();
		}

		if (C != '=')
		{
			throw ConfigParseException("Expected '=' after Key", Token.Line, Token.Pos, Token.Text.Size());
		}

		String Value;
		C = GetChar();
		char P = PeekChar();
		while (!IsEOL(C) && P != ';' && P != '#')
		{
			Value.Append(C);
			C = GetChar();
			P = PeekChar();
		}

		Key.LTrim();
		Key.RTrim();
		Value.LTrim();
		OnParsed_KeyValue(Key, Value);
	}
}
