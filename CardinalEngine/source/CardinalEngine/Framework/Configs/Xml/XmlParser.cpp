#include "CardinalEngine/Framework/Configs/Xml/XmlParser.h"

namespace CE
{
	bool XmlParser::Parse(ConfigParseException* Error /*= nullptr*/)
	{
		try
		{
			ConfigParseToken Token;
			while (GetToken(Token))
			{
				switch (Token.Type)
				{
				case ConfigParseToken::EType::Identifier:
					if (m_InElementBegin)
					{
						ProcessAttribute(Token);
						continue;
					}
					break;
				case ConfigParseToken::EType::Symbol:
					if (Token.Equals("<"))
					{
						ProcessElement(Token);
						continue;
					}
					else if (m_InElementBegin && Token.Equals(">"))
					{
						m_InElementBegin = false;
						OnParsed_BeginElement_End();
						continue;
					}
					else if (m_InProcessorInstruction && Token.Equals("?"))
					{
						ProcessInstructionEnd(Token);
						continue;
					}
					break;
				}

				ProcessText(Token);
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

	void XmlParser::ProcessElement(ConfigParseToken& Token)
	{
		if (GetToken(Token))
		{
			bool IsEnd = false;
			if (Token.Type == ConfigParseToken::EType::Symbol)
			{
				if (Token.Equals("/"))
				{
					if (!GetToken(Token))
					{
						throw ConfigParseException("Unexpected End of File", Token.Line, Token.Pos, Token.Text.Size());
					}
					IsEnd = true;
				}
				else if (Token.Equals("?"))
				{
					if (!GetToken(Token))
					{
						throw ConfigParseException("Unexpected End of File", Token.Line, Token.Pos, Token.Text.Size());
					}
					m_InProcessorInstruction = true;
				}
				else
				{
					throw ConfigParseException("Unexpected Symbol after Element begin '<'", Token.Line, Token.Pos, Token.Text.Size());
				}
			}

			String Name;
			if (Token.Type == ConfigParseToken::EType::Identifier)
			{
				Name = Token.Text;
			}
			else
			{
				throw ConfigParseException("Expected Identifier after Element begin '<'", Token.Line, Token.Pos, Token.Text.Size());
			}

			if (IsEnd)
			{
				if (GetToken(Token))
				{
					if (Token.Type == ConfigParseToken::EType::Symbol && Token.Equals(">"))
					{
						OnParsed_EndElement(Name);
						return;
					}
					else
					{
						throw ConfigParseException("Expected Symbol after Element End '>'", Token.Line, Token.Pos, Token.Text.Size());
					}
				}
			}
			else
			{
				OnParsed_BeginElement_Start(Name, m_InProcessorInstruction);
				m_InElementBegin = true;
				return;
			}
		}
		throw ConfigParseException("Unexpected End of File", Token.Line, Token.Pos, Token.Text.Size());
	}

	void XmlParser::ProcessAttribute(ConfigParseToken& Token)
	{
		String AttribName = Token.Text;

		if (GetToken(Token))
		{
			if (Token.Type == ConfigParseToken::EType::Symbol && Token.Equals("="))
			{
				if (GetToken(Token))
				{
					if (Token.Type == ConfigParseToken::EType::Const && Token.ConstType == ConfigParseToken::EConstType::Text)
					{
						OnParsed_Attribute(AttribName, Token.Text);
						return;
					}
					else
					{
						throw ConfigParseException("Expected Text Const after Attribute Equal", Token.Line, Token.Pos, Token.Text.Size());
					}
				}
			}
			else
			{
				throw ConfigParseException("Expected '=' after Attribute Name", Token.Line, Token.Pos, Token.Text.Size());
			}
		}

		throw ConfigParseException("Unexpected End of File", Token.Line, Token.Pos, Token.Text.Size());
	}

	void XmlParser::ProcessText(ConfigParseToken& Token)
	{
		String Text = Token.Text;

		char P = PeekChar();
		while (P != '\0' && P != '<')
		{
			Text.Append(P);
			GetChar();
			P = PeekChar();
		}

		Text.RTrim();
		OnParsed_Text(Text);
	}

	void XmlParser::ProcessInstructionEnd(ConfigParseToken& Token)
	{
		if (GetToken(Token))
		{
			if (Token.Type == ConfigParseToken::EType::Symbol && Token.Equals(">"))
			{
				m_InProcessorInstruction = false;
				OnParsed_BeginElement_End();
				return;
			}
			else
			{
				throw ConfigParseException("Expected Instruction End after '?'", Token.Line, Token.Pos, Token.Text.Size());
			}
		}

		throw ConfigParseException("Unexpected End of File", Token.Line, Token.Pos, Token.Text.Size());
	}
}
