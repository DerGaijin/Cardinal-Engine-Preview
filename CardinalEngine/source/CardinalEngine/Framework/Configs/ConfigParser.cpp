#include "CardinalEngine/Framework/Configs/ConfigParser.h"

namespace CE
{
	void ConfigParseToken::SetConstBool(bool Value)
	{
		Type = EType::Const;
		ConstType = EConstType::Bool;
		Val_Bool = Value;
	}

	void ConfigParseToken::SetConstInt(size_t Value)
	{
		Type = EType::Const;
		ConstType = EConstType::Integer;
		Val_Integer = Value;
	}

	void ConfigParseToken::SetConstDouble(double Value)
	{
		Type = EType::Const;
		ConstType = EConstType::Double;
		Val_Double = Value;
	}

	void ConfigParseToken::SetConstText()
	{
		Type = EType::Const;
		ConstType = EConstType::Text;
	}

	bool ConfigParser::IsEOL(char C)
	{
		return C == '\n' || C == '\r' || C == 0;
	}

	bool ConfigParser::IsWhitespace(char C)
	{
		return C == ' ' || C == '\t' || C == '\r' || C == '\n';
	}

	void ConfigParser::Reset()
	{
		m_CurrPos = 0;
		m_CurrLine = 1;

		if (m_StreamSource != nullptr)
		{
			m_StreamSource->clear();
			m_StreamSource->seekg(0, std::ios::beg);
		}
	}

	void ConfigParser::SetSource(String* StringSource)
	{
		m_StringSource = StringSource;
		m_StreamSource = nullptr;
	}

	void ConfigParser::SetSource(std::istream* StreamSource)
	{
		m_StreamSource = StreamSource;
	}

	char ConfigParser::PeekChar()
	{
		if (m_StringSource != nullptr)
		{
			return m_CurrPos < m_StringSource->Size() ? (*m_StringSource)[m_CurrPos] : '\0';
		}
		else if (m_StreamSource != nullptr)
		{
			return m_StreamSource->peek();
		}
		return '\0';
	}

	char ConfigParser::GetChar()
	{
		char C = '\0';
		if (m_StringSource != nullptr)
		{
			C = m_CurrPos < m_StringSource->Size() ? (*m_StringSource)[m_CurrPos] : '\0';
		}
		else if (m_StreamSource != nullptr)
		{
			C = m_StreamSource->get();
			if (C < 0)
			{
				C = '\0';
			}
		}
		if (IsEOL(C))
			m_CurrLine++;
		m_CurrPos++;
		return C;
	}

	char ConfigParser::GetLeadingChar(String* PreceedingWhitespaces /*= nullptr*/)
	{
		char C = GetChar();
		while (IsWhitespace(C))
		{
			if (PreceedingWhitespaces != nullptr)
				PreceedingWhitespaces->Append(C);
			C = GetChar();
		}
		return C;
	}

	bool ConfigParser::GetToken(ConfigParseToken& OutToken, bool NoConsts /*= false*/, bool NoRawStrings /*= false*/, String* PreceedingWhitespaces /*= nullptr*/)
	{
		char C = GetLeadingChar(PreceedingWhitespaces);
		if (C == '\0')
		{
			return false;
		}

		char P = PeekChar();
		OutToken.Type = ConfigParseToken::EType::None;
		OutToken.Text.Clear();
		OutToken.Pos = m_CurrPos - 1;
		OutToken.Line = m_CurrLine;
		OutToken.ConstType = ConfigParseToken::EConstType::Text;

		if (C == 'R' && P == '"')	// Raw String
		{
			GetChar();	// Skip '"' Peeked Char

			std::string Prefix;
			char C = GetChar();
			while (C != '(' && C != '\0')
			{
				Prefix += C;
				C = GetChar();
			}

			if (C != '(')
			{
				throw ConfigParseException("Incomplete Prefix on Raw string", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
			}

			Prefix.insert(0, ")");
			Prefix.push_back('"');

			C = GetChar();
			while (C != '\0')
			{
				if (C == '\\')
				{
					C = GetChar();
					if (IsEOL(C))
					{
						break;
					}
					else if (C == 'n')
					{
						C = '\n';
					}
				}
				OutToken.Text.Append(C);

				if (OutToken.Text.EndsWith(Prefix))
				{
					OutToken.Text.RemoveAt(OutToken.Text.Size() - Prefix.size(), Prefix.size());
					break;
				}

				C = GetChar();
			}

			if (C == '\0')
			{
				throw ConfigParseException("Unterminated raw string constant", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
			}

			OutToken.SetConstText();
			return true;
		}
		else if (C == '"')	// Normal String
		{
			C = GetChar();
			while ((C != '"') && !IsEOL(C))
			{
				if (C == '\\')
				{
					C = GetChar();
					if (IsEOL(C))
					{
						break;
					}
					else if (C == 'n')
					{
						C = '\n';
					}
				}
				OutToken.Text.Append(C);
				C = GetChar();
			}

			if (C != '"')
			{
				throw ConfigParseException("Unterminated string constant", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
			}

			OutToken.SetConstText();
			return true;
		}
		else if (C == '\'')	// Char literal
		{
			C = GetChar();
			while ((C != '\'') && C != '\0')
			{
				if (OutToken.Text.Size() == 4)
				{
					throw ConfigParseException("Too many characters in char literal", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
				}

				if (C == '\\')
				{
					C = GetChar();
					switch (C)
					{
					case 'n':
						C = '\n';
						break;
					case 't':
						C = '\t';
						break;
					case 'v':
						C = '\v';
						break;
					case 'b':
						C = '\b';
						break;
					case 'r':
						C = '\r';
						break;
					case 'f':
						C = '\f';
						break;
					case 'a':
						C = '\a';
						break;
					case '\\':
						C = '\\';
						break;
					case '?':
						C = '\?';
						break;
					case '\'':
						C = '\'';
						break;
					case '"':
						C = '\"';
						break;
					case 'x':
					{
						std::string HexStr = "0";
						HexStr += C;
						for (char i = 0; i < 4; i++)
						{
							P = std::toupper(PeekChar());
							if ((P >= '0' && P <= '9') || (P >= 'A' && P <= 'F'))
							{
								HexStr = GetChar();
							}
							else
							{
								break;
							}
						}

						char* End = (char*)HexStr.data() + HexStr.size();
						C = (char)std::strtol(HexStr.c_str(), &End, 0);
						break;
					}
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						std::string OctStr;
						OctStr += C;
						for (char i = 0; i < 2; i++)
						{
							P = PeekChar();
							if (P >= '0' && P <= '7')
							{
								OctStr = GetChar();
							}
							else
							{
								break;
							}
						}

						char* End = (char*)OctStr.data() + OctStr.size();
						C = (char)std::strtol(OctStr.c_str(), &End, 8);

						break;
					}
					default:
						break;
					}
				}
				OutToken.Text.Append(C);

				C = GetChar();
			}

			if (C != '\'')
			{
				throw ConfigParseException("Unterminated char literal", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
			}

			int Val = 0;
			if (OutToken.Text.Size() == 4)
			{
				Val = int((unsigned char)(OutToken.Text[0]) << 24 |
					(unsigned char)(OutToken.Text[1]) << 16 |
					(unsigned char)(OutToken.Text[2]) << 8 |
					(unsigned char)(OutToken.Text[3]));
			}
			else if (OutToken.Text.Size() == 3)
			{
				Val = int((unsigned char)(OutToken.Text[0]) << 16 |
					(unsigned char)(OutToken.Text[1]) << 8 |
					(unsigned char)(OutToken.Text[2]));
			}
			else if (OutToken.Text.Size() == 2)
			{
				Val = int((unsigned char)(OutToken.Text[0]) << 8 |
					(unsigned char)(OutToken.Text[1]));
			}
			else
			{
				Val = int((unsigned char)(OutToken.Text[0]));
			}

			OutToken.SetConstInt(Val);
			return true;
		}
		else if ((C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z') || C == '_')
		{
			OutToken.Type = ConfigParseToken::EType::Identifier;

			while (true)
			{
				OutToken.Text.Append(C);
				P = PeekChar();
				if (((P >= 'A') && (P <= 'Z')) || ((P >= 'a') && (P <= 'z')) || ((P >= '0') && (P <= '9')) || (P == '_'))
					C = GetChar();
				else
					break;
			}

			if (!NoConsts)
			{
				if (OutToken.Text == "true")
				{
					OutToken.SetConstBool(true);
					return true;
				}
				else if (OutToken.Text == "false")
				{
					OutToken.SetConstBool(false);
					return true;
				}
			}

			return true;
		}
		else if (!NoConsts && (C >= '0' && C <= '9') || ((C == '+' || C == '-') && (P >= '0' && P <= '9')))	// Const
		{
			OutToken.Type = ConfigParseToken::EType::Const;

			//@Note: It is possible to write a number like 10'000 so we ignore the separators
			if (C == '0' && (P == 'x' || P == 'X'))
			{
				OutToken.Text.Append(C);
				OutToken.Text.Append(std::tolower(GetChar()));
				while (true)
				{
					P = std::toupper(PeekChar());
					if ((P >= '0' && P <= '9') || (P >= 'A' && P <= 'F'))
					{
						OutToken.Text.Append(std::toupper(GetChar()));
					}
					else if (P == '\'')
					{
						C = GetChar();
					}
					else
					{
						break;
					}
				}

				char* End = (char*)OutToken.Text.Data() + OutToken.Text.Size();
				OutToken.SetConstInt(std::strtoll(OutToken.Text.Data(), &End, 0));
			}
			else if (C == '0' && (P == 'b' || P == 'B'))
			{
				GetChar();
				while (true)
				{
					P = PeekChar();
					if (P >= '0' && P <= '1')
					{
						OutToken.Text.Append(GetChar());
					}
					else if (P == '\'')
					{
						C = GetChar();
					}
					else
					{
						break;
					}
				}

				char* End = (char*)OutToken.Text.Data() + OutToken.Text.Size();
				OutToken.SetConstInt(std::strtoll(OutToken.Text.Data(), &End, 2));
			}
			else
			{
				OutToken.Text.Append(C);

				bool IsFloat = false;
				while (true)
				{
					P = PeekChar();
					if ((P >= '0' && P <= '9') || P == '.')
					{
						if (P == '.')
						{
							if (IsFloat)
							{
								throw ConfigParseException("Float/Double separator already found", OutToken.Line, OutToken.Pos, OutToken.Text.Size());
							}
							else
							{
								IsFloat = true;
							}
						}
						OutToken.Text.Append(GetChar());
					}
					else if (P == '\'')
					{
						C = GetChar();
					}
					else
					{
						break;
					}
				}
				if (IsFloat && (P == 'f' || P == 'F'))
				{
					OutToken.Text.Append(P);
					C = GetChar();
				}

				if (IsFloat)
				{
					OutToken.SetConstDouble(std::atof(OutToken.Text.Data()));
				}
				else
				{
					OutToken.SetConstInt(std::atoll(OutToken.Text.Data()));
				}
			}

			return true;
		}
		else	// Symbol
		{
			OutToken.Type = ConfigParseToken::EType::Symbol;
			// Symbol.
			OutToken.Text = C;

			// Handle special 2-character symbols.
#define PAIR(cc,dd) ((C==cc)&&(d==dd)) /* Comparison macro for convenience */
			char d = PeekChar();
			if
				(PAIR('<', '<')
					|| (PAIR('>', '>'))// && (bParseTemplateClosingBracket != ESymbolParseOption::CloseTemplateBracket))
					|| PAIR('!', '=')
					|| PAIR('<', '=')
					|| PAIR('>', '=')
					|| PAIR('+', '+')
					|| PAIR('-', '-')
					|| PAIR('+', '=')
					|| PAIR('-', '=')
					|| PAIR('*', '=')
					|| PAIR('/', '=')
					|| PAIR('&', '&')
					|| PAIR('|', '|')
					|| PAIR('^', '^')
					|| PAIR('=', '=')
					|| PAIR('*', '*')
					|| PAIR('~', '=')
					|| PAIR(':', ':')
					|| PAIR('/', '/')
					|| PAIR('/', '*')
					|| PAIR('*', '/')
					)
			{
				GetChar();
				OutToken.Text.Append(d);
				if (C == '>' && d == '>')
				{
					if (PeekChar() == '>')
					{
						GetChar();
						OutToken.Text.Append('>');
					}
				}
			}
#undef PAIR
			return true;
		}
		return false;
	}
}
