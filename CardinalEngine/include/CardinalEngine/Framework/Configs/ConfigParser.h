#pragma once
#include "../Core/Headers.h"

namespace CE
{
	class CE_API ConfigParseException : public std::exception
	{
	public:
		ConfigParseException() = default;
		ConfigParseException(const std::string& Msg, size_t Line, size_t Pos, size_t Count) : Msg(Msg), std::exception(Msg.c_str()), Line(Line), Pos(Pos), Count(Count) {}

	public:
		size_t Line = 0;
		size_t Pos = 0;
		size_t Count = 0;

	private:
		std::string Msg;
	};


	class CE_API ConfigParseToken
	{
	public:
		enum class EType
		{
			None,
			Identifier,
			Const,
			Symbol,
		};

		enum class EConstType
		{
			Text,
			Double,
			Integer,
			Bool,
		};

	public:
		EType Type = EType::None;
		size_t Pos = 0;
		size_t Line = 1;
		String Text;

		EConstType ConstType = EConstType::Text;
		union
		{
			size_t Val_Integer = 0;
			bool Val_Bool;
			double Val_Double;
		};

	public:
		bool Equals(const char* InStr) const
		{
			return Text == InStr;
		}

		void SetConstBool(bool Value);

		void SetConstInt(size_t Value);

		void SetConstDouble(double Value);

		void SetConstText();
	};


	class CE_API ConfigParser
	{
	public:
		// Returns true if the Char is End Of Line
		static bool IsEOL(char C);

		// Returns true if the Char is a Whitespace
		static bool IsWhitespace(char C);

	public:
		// Destructor
		virtual ~ConfigParser() = default;

		// Resets the Config Parser
		virtual void Reset();

		// Set the Source for the Parser to a String
		void SetSource(String* StringSource);

		// Set the Source for the Parser to a Stream
		void SetSource(std::istream* StreamSource);

		// Returns the Next Char without increasing the Pos
		virtual char PeekChar();

		// Returns the Next Char
		virtual char GetChar();

		// Returns the Next Char that is not whitespace
		virtual char GetLeadingChar(String* PreceedingWhitespaces = nullptr);

		// Returns the Next Token
		virtual bool GetToken(ConfigParseToken& OutToken, bool NoConsts = false, bool NoRawStrings = false, String* PreceedingWhitespaces = nullptr);

	protected:
		String* m_StringSource = nullptr;
		std::istream* m_StreamSource = nullptr;

		size_t m_CurrPos = 0;
		size_t m_CurrLine = 1;
	};
}
