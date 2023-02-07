#pragma once
#include "../Definitions.h"
#include "../Logging/Log.h"
#include "../Math/Fundamentals.h"

#include <xmemory>
#include <string>
#include <locale>
#include <codecvt>

namespace CE
{
	template<typename T>
	struct BasicStringFunctions;

	template<>
	struct BasicStringFunctions<char>
	{
		static char* Copy(char* const To, const char* From, const size_t Count) noexcept
		{
			return static_cast<char*>(memcpy(To, From, Count));
		}
		static char* Move(char* const To, const char* From, const size_t Count) noexcept
		{
			return static_cast<char*>(memmove(To, From, Count));
		}
		static char* Set(char* const Str, const size_t Count, const char Ch) noexcept
		{
			return static_cast<char*>(memset(Str, Ch, Count));
		}
		static size_t Length(const char* Str) noexcept
		{
			return strlen(reinterpret_cast<const char*>(Str));
		}
		static int Compare(const char* First, const char* Second, const size_t Count) noexcept
		{
			return memcmp(First, Second, Count);
		}
		static const char* Find(const char* Str, const size_t Count, const char& Ch) noexcept
		{
			return static_cast<const char*>(memchr(Str, Ch, Count));
		}
		static const char* StrStr(const char* Str, const char* ToFind) noexcept
		{
			return strstr(Str, ToFind);
		}
		template<typename... ARGS>
		static int sPrintf(char* Buffer, size_t BufferCount, const char* Format, ARGS... Args)
		{
			return snprintf(Buffer, BufferCount, Format, Args...);
		}
		static int32 StrToInt32(const char* Str, char** Last, int32 Base)
		{
			return strtol(Str, Last, Base);
		}
		static int64 StrToInt64(const char* Str, char** Last, int32 Base)
		{
			return strtoll(Str, Last, Base);
		}
		static uint32 StrToUInt32(const char* Str, char** Last, int32 Base)
		{
			return strtoul(Str, Last, Base);
		}
		static uint64 StrToUInt64(const char* Str, char** Last, int32 Base)
		{
			return strtoull(Str, Last, Base);
		}
		static float StrToFloat(const char* Str, char** Last)
		{
			return strtof(Str, Last);
		}
		static double StrToDouble(const char* Str, char** Last)
		{
			return strtod(Str, Last);
		}
		static constexpr const char* BoolTrue = "true";
		static constexpr const char* BoolFalse = "false";
	};

	template<>
	struct BasicStringFunctions<wchar_t>
	{
		static wchar_t* Copy(wchar_t* const To, const wchar_t* From, const size_t Count) noexcept
		{
			return reinterpret_cast<wchar_t*>(wmemcpy(reinterpret_cast<wchar_t*>(To), reinterpret_cast<const wchar_t*>(From), Count));
		}
		static wchar_t* Move(wchar_t* const To, const wchar_t* From, const size_t Count) noexcept
		{
			return reinterpret_cast<wchar_t*>(wmemmove(reinterpret_cast<wchar_t*>(To), reinterpret_cast<const wchar_t*>(From), Count));
		}
		static wchar_t* Set(wchar_t* const Str, const size_t Count, const wchar_t Ch) noexcept
		{
			return reinterpret_cast<wchar_t*>(wmemset(reinterpret_cast<wchar_t*>(Str), Ch, Count));
		}
		static size_t Length(const wchar_t* Str) noexcept
		{
			return wcslen(reinterpret_cast<const wchar_t*>(Str));
		}
		static int Compare(const wchar_t* First, const wchar_t* Second, const size_t Count) noexcept
		{
			return wmemcmp(reinterpret_cast<const wchar_t*>(First), reinterpret_cast<const wchar_t*>(Second), Count);
		}
		static const wchar_t* Find(const wchar_t* Str, const size_t Count, const wchar_t& Ch) noexcept
		{
			return reinterpret_cast<const wchar_t*>(wmemchr(reinterpret_cast<const wchar_t*>(Str), Ch, Count));
		}
		static const wchar_t* StrStr(const wchar_t* Str, const wchar_t* ToFind) noexcept
		{
			return wcsstr(Str, ToFind);
		}
		template<typename... ARGS>
		static int sPrintf(wchar_t* Buffer, size_t BufferCount, const wchar_t* Format, ARGS... Args)
		{
			return swprintf(Buffer, BufferCount, Format, Args...);
		}
		static int32 StrToInt32(const wchar_t* Str, wchar_t** Last, int32 Base)
		{
			return wcstol(Str, Last, Base);
		}
		static int64 StrToInt64(const wchar_t* Str, wchar_t** Last, int32 Base)
		{
			return wcstoll(Str, Last, Base);
		}
		static uint32 StrToUInt32(const wchar_t* Str, wchar_t** Last, int32 Base)
		{
			return wcstoul(Str, Last, Base);
		}
		static uint64 StrToUInt64(const wchar_t* Str, wchar_t** Last, int32 Base)
		{
			return wcstoull(Str, Last, Base);
		}
		static float StrToFloat(const wchar_t* Str, wchar_t** Last)
		{
			return wcstof(Str, Last);
		}
		static double StrToDouble(const wchar_t* Str, wchar_t** Last)
		{
			return wcstod(Str, Last);
		}
		static constexpr const wchar_t* BoolTrue = L"true";
		static constexpr const wchar_t* BoolFalse = L"false";
	};

	template<typename T, typename F>
	struct StringConverter;

	template<typename C, typename Alloc = std::allocator<C>>
	class BasicString
	{
	public:
		typedef C CharType;
		typedef size_t SizeType;
		typedef BasicStringFunctions<CharType> CharFunctions;
		typedef std::allocator_traits<Alloc> AllocatorTraits;

		static constexpr SizeType NPos = (SizeType)-1;

	public:
		// Formats a String with given Args
		template<typename... ARGS>
		static BasicString Format(const BasicString& Fmt, ARGS&&... Args)
		{
			int RequiredSize = CharFunctions::sPrintf(nullptr, 0, Fmt.Data(), Args...) + 1;
			if (RequiredSize <= 0)
			{
				return Fmt;
			}
			auto Size = static_cast<SizeType>(RequiredSize);

			BasicString Result;
			Result.Clear(Size);
			CharFunctions::sPrintf(Result.Data(), Size, Fmt.Data(), Args...);
			Result.m_Size = Size - 1;	//@Note: Removes the null terminator added by sPrintf
			return Result;
		}

		template<typename C, typename Allocator>
		static BasicString Convert(const BasicString<C, Allocator>& From)
		{
			BasicString<CharType, Alloc> Result;
			StringConverter<CharType, C>::Convert(Result, From);
			return Result;
		}

	public:
		// Default Constructor
		BasicString() = default;
		
		// Copy Constructor
		BasicString(const BasicString& Other)
		{
			*this = std::forward<const BasicString&>(Other);
		}

		// Copy Operator
		BasicString& operator=(const BasicString& Other)
		{
			Clear();
			m_Allocator = Other.m_Allocator;
			InsertData(0, Other.Size());
			CharFunctions::Copy(GetDataPtr(), Other.GetDataPtr(), Other.m_Size);
			return *this;
		}

		// Move Constructor
		BasicString(BasicString&& Other) noexcept
		{
			*this = std::forward<BasicString&&>(Other);
		}

		// Move Operator
		BasicString& operator=(BasicString&& Other) noexcept
		{
			CharType Temp[SmallBuffSize] = { 0 };
			CharFunctions::Move(Temp, m_Small, SmallBuffSize);
			CharFunctions::Move(m_Small, Other.m_Small, SmallBuffSize);
			CharFunctions::Move(Other.m_Small, Temp, SmallBuffSize);
			std::swap(m_Size, Other.m_Size);
			std::swap(m_Reserved, Other.m_Reserved);
			std::swap(m_Allocator, Other.m_Allocator);
			Other.Clear();
			return *this;
		}

		// Raw String Constructor
		BasicString(const CharType* Str)
		{
			*this = std::forward<const CharType*>(Str);
		}

		// Raw String Operator
		BasicString& operator=(const CharType* Str)
		{
			Clear();
			Insert(0, Str);
			return *this;
		}

		// Raw String Constructor with Size
		BasicString(const CharType* Str, SizeType StrSize)
		{
			Append(Str, StrSize);
		}

		// std::string Constructor
		template<typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
		BasicString(const std::basic_string<CharType, Traits, Alloc>& Str)
		{
			*this = std::forward<const std::basic_string<CharType, Traits, Alloc>&>(Str);
		}

		// std::string Operator
		template<typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
		BasicString& operator=(const std::basic_string<CharType, Traits, Alloc>& Str)
		{
			Clear(Str.size());
			Insert(0, Str);
			return *this;
		}

		// Char Constructor
		BasicString(const CharType& C)
		{
			*this = std::forward<const CharType&>(C);
		}

		// Char Operator
		BasicString& operator=(const CharType& C)
		{
			Clear(1);
			Insert(0, C);
			return *this;
		}

		// Destructor
		~BasicString()
		{
			if (m_Reserved >= SmallBuffSize)
			{
				AllocatorTraits::deallocate(m_Allocator, GetDataPtr(), m_Reserved + 1);
			}
		}

	public:
		// Clears the String and Reserves memory
		void Clear(SizeType Reserve = 0)
		{
			m_Size = 0;
			if (Reserve != m_Reserved)
			{
				if (m_Reserved >= SmallBuffSize)
				{
					AllocatorTraits::deallocate(m_Allocator, GetDataPtr(), m_Reserved + 1);
				}
				m_Reserved = Reserve;
				if (m_Reserved >= SmallBuffSize)
				{
					m_Data = AllocatorTraits::allocate(m_Allocator, m_Reserved + 1);
				}
			}
			GetDataPtr()[0] = 0;
		}

		// Reserves Memory
		void Reserve(SizeType Reserve)
		{
			if (m_Reserved != Reserve && Reserve >= m_Size)
			{
				if (Reserve >= SmallBuffSize)
				{
					CharType* NewData = AllocatorTraits::allocate(m_Allocator, Reserve + 1);
					if (m_Size > 0)
					{
						CharFunctions::Move(NewData, GetDataPtr(), m_Size + 1);
					}
					if (m_Reserved >= SmallBuffSize)
					{
						AllocatorTraits::deallocate(m_Allocator, GetDataPtr(), m_Reserved + 1);
					}
					m_Data = NewData;
					m_Reserved = Reserve;
					m_Data[m_Size] = 0;
				}
				else
				{
					if (m_Reserved >= SmallBuffSize)
					{
						AllocatorTraits::deallocate(m_Allocator, GetDataPtr(), m_Reserved + 1);
					}
					m_Reserved = SmallBuffSize - 1;
				}
			}
		}

		// Shrinks the memory to fit the Size
		void Shrink()
		{
			Reserve(m_Size);
		}

		// Appends a String
		void Append(const BasicString& String)
		{
			Append(String.Data(), String.Size());
		}

		// Appends a Raw String
		void Append(const CharType* Str)
		{
			Insert(m_Size, Str);
		}

		// Appends a Raw String with Size
		void Append(const CharType* Str, SizeType StrSize)
		{
			Insert(m_Size, Str, StrSize);
		}

		// Appends Chars
		void Append(const CharType& C, SizeType Count = 1)
		{
			Insert(m_Size, C, Count);
		}

		// Appends std::string
		template<typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
		void Append(const std::basic_string<CharType, Traits, Alloc>& Str)
		{
			Insert(m_Size, std::forward<const std::basic_string<CharType, Traits, Alloc>&>(Str));
		}

		// Inserts String at Index
		void Insert(SizeType Index, const BasicString& String)
		{
			Insert(Index, String.Data(), String.Size());
		}

		// Inserts Raw String at Index
		void Insert(SizeType Index, const CharType* Str)
		{
			CE_CHECK(Str != nullptr);
			Insert(Index, Str, CharFunctions::Length(Str));
		}

		// Inserts Raw String with Size at Index
		void Insert(SizeType Index, const CharType* Str, SizeType StrSize)
		{
			CE_CHECK(Str != nullptr);
			CharType* Added = InsertData(Index, StrSize);
			if (Added != nullptr)
			{
				CharFunctions::Move(Added, Str, StrSize);
			}
		}

		// Inserts std::string at Index
		template<typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
		void Insert(SizeType Index, const std::basic_string<CharType, Traits, Alloc>& Str)
		{
			Insert(Index, Str.data(), Str.size());
		}

		// Inserts Chars at Index
		void Insert(SizeType Index, const CharType& C, SizeType Count = 1)
		{
			CharType* Added = InsertData(Index, Count);
			if (Added != nullptr)
			{
				CharFunctions::Set(Added, Count, C);
			}
		}

		// Removes Chars at Index
		void RemoveAt(SizeType Index, SizeType Count = 1)
		{
			if (IsValidIndex(Index) && Count > 0)
			{
				if (Count > m_Size - Index)
				{
					Count = m_Size - Index;
				}
				CharFunctions::Move(GetDataPtr() + Index, GetDataPtr() + Index + Count, (m_Size - (Index + Count)) + 1);
				m_Size -= Count;
			}
		}
		
		// Replaces a String with another String
		SizeType Replace(const BasicString& ToReplace, const BasicString& With, SizeType Count = 1)
		{
			if (Count == 0 || ToReplace.Size() == 0)
			{
				return 0;
			}

			SizeType Replaced = 0;

			SizeType Index = 0;
			while (Index < m_Size && Replaced < Count)
			{
				SizeType FoundAt = Find(ToReplace, Index);
				if (FoundAt == NPos)
				{
					break;
				}

				if (ToReplace.Size() == With.Size())
				{
					CharFunctions::Copy(GetDataPtr() + FoundAt, With.GetDataPtr(), With.Size());
				}
				else if (ToReplace.Size() > With.Size())
				{
					RemoveAt(FoundAt, ToReplace.Size() - With.Size());
					CharFunctions::Copy(GetDataPtr() + FoundAt, With.GetDataPtr(), With.Size());
				}
				else
				{
					SizeType Inserted = With.Size() - ToReplace.Size();
					Insert(FoundAt, With.Data(), Inserted);
					CharFunctions::Copy(GetDataPtr() + FoundAt + Inserted, With.GetDataPtr() + Inserted, With.Size() - Inserted);
				}

				Index = FoundAt + With.Size();
				Replaced++;
			}

			return Replaced;
		}

		// Replaces all Strings with another String
		SizeType ReplaceAll(const BasicString& ToReplace, const BasicString& With)
		{
			return Replace(ToReplace, With, m_Size);
		}

		// Transforms the String to Uppercase
		void ToUpper()
		{
			for (auto It = begin(); It != end(); It++)
			{
				*It = std::toupper(*It);
			}
		}
		
		// Transforms the String to Lowercase
		void ToLower()
		{
			for (auto It = begin(); It != end(); It++)
			{
				*It = std::tolower(*It);
			}
		}

		// Removes all whitespaces from Left
		void LTrim()
		{
			SizeType Index = 0;
			while (Index < m_Size)
			{
				if (!std::isspace(GetDataPtr()[Index]))
				{
					break;
				}
				Index++;
			}

			if (Index > 0)
			{
				RemoveAt(0, Index);
			}
		}

		// Removes all whitespaces from Right
		void RTrim()
		{
			SizeType Index = m_Size;
			while (Index > 0)
			{
				if (!std::isspace(GetDataPtr()[Index - 1]))
				{
					break;
				}
				Index--;
			}

			if (Index != m_Size)
			{
				RemoveAt(Index, m_Size - Index);
			}
		}

		// Returns the Size
		SizeType Size() const
		{
			return m_Size;
		}

		// Returns the Reserved memory
		SizeType Reserved() const
		{
			return m_Reserved;
		}

		// Returns the Pointer
		CharType* Data()
		{
			return GetDataPtr();
		}

		// Returns the Pointer as const
		const CharType* Data() const
		{
			return GetDataPtr();
		}

		// Returns true if the Index is a valid Index
		bool IsValidIndex(SizeType Index) const
		{
			return Index >= 0 && Index < m_Size;
		}

		// Returns a Substring at Offset
		BasicString SubString(SizeType Offset = 0) const
		{
			if (!IsValidIndex(Offset))
			{
				return BasicString();
			}
			return BasicString(GetDataPtr() + Offset, m_Size - Offset);
		}

		// Returns a Substring at Offset with Count
		BasicString SubString(SizeType Offset, SizeType Count) const
		{
			if (!IsValidIndex(Offset))
			{
				return BasicString();
			}
			return BasicString(GetDataPtr() + Offset, Count > m_Size - Offset ? m_Size - Offset : Count);
		}

		// Finds a Str
		SizeType Find(const BasicString& Str, SizeType Offset = 0) const
		{
			return Find(Str.Data(), Str.Size(), Offset, m_Size);
		}
		
		// Finds a Str
		SizeType Find(const BasicString& Str, SizeType Offset, SizeType Count) const
		{
			return Find(Str.Data(), Str.Size(), Offset, Count);
		}

		// Finds a Str
		SizeType Find(const CharType* Str, SizeType Offset = 0) const
		{
			return Find(Str, CharFunctions::Length(Str), Offset, m_Size);
		}
		
		// Finds a Str
		SizeType Find(const CharType* Str, SizeType StrSize, SizeType Offset) const
		{
			return Find(Str, StrSize, Offset, m_Size);
		}

		// Finds a Str
		SizeType Find(const CharType* Str, SizeType StrSize, SizeType Offset, SizeType Count) const
		{
			if (Offset >= m_Size || StrSize == 0 || StrSize >= m_Size)
			{
				return NPos;
			}

			const CharType* Found = CharFunctions::StrStr(GetDataPtr() + Offset, Str);
			if (Found != nullptr)
			{
				size_t FoundIdx = Found - GetDataPtr();
				return FoundIdx < Offset + Count ? FoundIdx : NPos;
			}

			return NPos;
		}

		// Returns true if the String start with Str
		bool StartsWith(const BasicString& Str) const
		{
			return StartsWith(Str.Data(), Str.Size());
		}
		
		// Returns true if the String start with Str
		bool StartsWith(const CharType* Str) const
		{
			return StartsWith(Str, CharFunctions::Length(Str));
		}

		// Returns true if the String start with Str
		bool StartsWith(const CharType* Str, SizeType StrSize) const
		{
			return m_Size >= StrSize && CharFunctions::Compare(GetDataPtr(), Str, StrSize) == 0;
		}

		// Returns true if the String ends with Str
		bool EndsWith(const BasicString& Str) const
		{
			return EndsWith(Str.Data(), Str.Size());
		}

		// Returns true if the String ends with Str
		bool EndsWith(const CharType* Str) const
		{
			return EndsWith(Str, CharFunctions::Length(Str));
		}

		// Returns true if the String ends with Str
		bool EndsWith(const CharType* Str, SizeType StrSize) const
		{
			SizeType Diff = m_Size - StrSize;
			return m_Size >= StrSize && CharFunctions::Compare(GetDataPtr() + Diff, Str, StrSize) == 0;
		}

		// Returns true if the String is a bool Value
		bool IsBool() const
		{
			bool Value = false;
			return ToBool(Value);
		}

		// Tries to convert the String to a bool Value
		bool ToBool(bool& Value) const
		{
			BasicString Upper = *this;
			Upper.ToLower();
			if (Upper == CharFunctions::BoolTrue)
			{
				Value = true;
				return true;
			}
			else if (Upper == CharFunctions::BoolFalse)
			{
				Value = false;
				return true;
			}
			return false;
		}

		// Returns true if the String is a integral Value
		bool IsIntegral() const
		{
			int64 Value = 0;
			return ToIntegral(Value);
		}
		
		// Tries to convert the String to a integral Value
		bool ToIntegral(int32& Value) const
		{
			CharType* Last = nullptr;
			Value = CharFunctions::StrToInt32(GetDataPtr(), &Last, 10);
			if (Value != 0)
			{
				return true;
			}
			else if (Size() > 0 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			return false;
		}

		// Tries to convert the String to a integral Value
		bool ToIntegral(int64& Value) const
		{
			CharType* Last = nullptr;
			Value = CharFunctions::StrToInt64(GetDataPtr(), &Last, 10);
			if (Value != 0)
			{
				return true;
			}
			else if (Size() > 0 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			return false;
		}

		// Returns true if the String is a unsigned integral Value
		bool IsUnsignedIntegral() const
		{
			uint64 Value = 0;
			return ToUnsignedIntegral(Value);
		}

		// Tries to convert the String to a unsigned integral Value
		bool ToUnsignedIntegral(uint32& Value) const
		{
			if (Size() > 0 && GetDataPtr()[0] == '-')
			{
				return false;
			}

			CharType* Last = nullptr;
			Value = CharFunctions::StrToUInt32(GetDataPtr(), &Last, 10);
			if (Value != 0)
			{
				return true;
			}
			else if (Size() > 0 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			return false;
		}

		// Tries to convert the String to a unsigned integral Value
		bool ToUnsignedIntegral(uint64& Value) const
		{
			if (Size() > 0 && GetDataPtr()[0] == '-')
			{
				return false;
			}

			CharType* Last = nullptr;
			Value = CharFunctions::StrToUInt64(GetDataPtr(), &Last, 10);
			if (Value != 0)
			{
				return true;
			}
			else if (Size() > 0 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			return false;
		}

		// Returns true if the String is a floating point Value
		bool IsFloat() const
		{
			double Value = 0;
			return ToFloat(Value);
		}

		// Tries to convert the String to a floating point Value
		bool ToFloat(float& Value) const
		{
			CharType* Last = nullptr;
			Value = CharFunctions::StrToFloat(GetDataPtr(), &Last);
			if (Value != 0.0f)
			{
				return true;
			}
			else if (Size() == 1 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			else if (Size() == 2 && GetDataPtr()[0] == '0' && GetDataPtr()[1] == '.')
			{
				return true;
			}
			else if (Size() > 2 && GetDataPtr()[0] == '0' && GetDataPtr()[1] == '.' && GetDataPtr()[2] == '0')
			{
				return true;
			}
			return false;
		}

		// Tries to convert the String to a floating point Value
		bool ToFloat(double& Value) const
		{
			CharType* Last = nullptr;
			Value = CharFunctions::StrToDouble(GetDataPtr(), &Last);
			if (Value != 0.0f)
			{
				return true;
			}
			else if (Size() == 1 && GetDataPtr()[0] == '0')
			{
				return true;
			}
			else if (Size() == 2 && GetDataPtr()[0] == '0' && GetDataPtr()[1] == '.')
			{
				return true;
			}
			else if (Size() > 2 && GetDataPtr()[0] == '0' && GetDataPtr()[1] == '.' && GetDataPtr()[2] == '0')
			{
				return true;
			}
			return false;
		}

	public:
		// Returns the Char at Index
		const CharType& operator[](SizeType Index) const
		{
			CE_ASSERT(IsValidIndex(Index), "String Index out of bounds");
			return GetDataPtr()[Index];
		}

		// Returns the Char at Index
		CharType& operator[](SizeType Index)
		{
			CE_ASSERT(IsValidIndex(Index), "String Index out of bounds");
			return GetDataPtr()[Index];
		}

		// returns the begin iterator
		CharType* begin()
		{
			return GetDataPtr();
		}

		// returns the begin iterator as const
		const CharType* begin() const
		{
			return GetDataPtr();
		}

		// returns the end iterator
		CharType* end()
		{
			return GetDataPtr() + m_Size;
		}

		// returns the end iterator as const
		const CharType* end() const
		{
			return GetDataPtr() + m_Size;
		}

		// Appends Str
		BasicString& operator+=(const BasicString& Str)
		{
			Append(Str);
			return *this;
		}

		// Appends RawStr
		BasicString& operator+=(const CharType* RawStr)
		{
			Append(RawStr);
			return *this;
		}

		// Returns combination of this and Other
		BasicString operator+(const BasicString& Str) const
		{
			return BasicString(*this) += Str;
		}

		// Returns combination of this and Other
		BasicString operator+(const CharType* RawStr) const
		{
			return BasicString(*this) += RawStr;
		}

		// Equal Operator
		bool operator==(const BasicString& Str) const
		{
			return Str.Size() == m_Size && CharFunctions::Compare(GetDataPtr(), Str.Data(), Str.Size()) == 0;
		}

		// Equal Operator
		bool operator==(const CharType* Str) const
		{
			SizeType StrSize = CharFunctions::Length(Str);
			return StrSize == m_Size && CharFunctions::Compare(GetDataPtr(), Str, StrSize) == 0;
		}

		// Unequal Operator
		bool operator!=(const BasicString& Str) const
		{
			return !(*this == Str);
		}

		// Unequal Operator
		bool operator!=(const CharType* Str) const
		{
			return !(*this == Str);
		}

		// smaller Operator
		bool operator<(const BasicString& Str) const
		{
			if (Str.m_Size == m_Size)
			{
				return CharFunctions::Compare(GetDataPtr(), Str.GetDataPtr(), m_Size) < 0;
			}
			return Str.m_Size < m_Size;
		}

		// smaller Operator
		bool operator<(const CharType* Str) const
		{
			const SizeType Size = CharFunctions::Length(Str);
			if (Size == m_Size)
			{
				return CharFunctions::Compare(GetDataPtr(), Str, Size) < 0;
			}
			return Size < m_Size;
		}

	private:
		// Calculates the new Reserved Size
		SizeType CalcReserved(SizeType MinReserved) const
		{
			static constexpr SizeType AllocMask = sizeof(CharType) <= 1 ? 15 : sizeof(CharType) <= 2 ? 7 : sizeof(CharType) <= 4 ? 3 : sizeof(CharType) <= 8 ? 1 : 0;
			const SizeType Mask = MinReserved | AllocMask;

			const SizeType Geometric = std::max(Mask, m_Reserved + m_Reserved / 2);
			if (Geometric > MinReserved)
			{
				return Geometric;
			}
			return MinReserved;
		}

		// Inserts data without initializing it
		CharType* InsertData(SizeType Index, SizeType Count)
		{
			if (IsValidIndex(Index) || Index == m_Size)
			{
				if ((m_Size + Count) + 1 > SmallBuffSize)
				{
					if (m_Reserved < m_Size + Count)
					{
						SizeType NewReserved = CalcReserved(m_Size + Count);
						CharType* NewData = AllocatorTraits::allocate(m_Allocator, NewReserved + 1);
						if (m_Size > 0)
						{
							CharFunctions::Move(NewData, GetDataPtr(), Index);
							CharFunctions::Move(NewData + Index + Count, GetDataPtr() + Index, (m_Size - Index) + 1);
						}
						if (m_Reserved >= SmallBuffSize)
						{
							AllocatorTraits::deallocate(m_Allocator, GetDataPtr(), m_Reserved + 1);
						}
						m_Data = NewData;
						m_Reserved = NewReserved;
						m_Size += Count;
						m_Data[m_Size] = 0;
					}
					else
					{
						CharFunctions::Move(GetDataPtr() + Index + Count, GetDataPtr() + Index, (m_Size - Index) + 1);
						m_Size += Count;
					}
				}
				else
				{
					CharFunctions::Move(GetDataPtr() + Index + Count, GetDataPtr() + Index, (m_Size - Index) + 1);
					m_Size += Count;
				}
				return GetDataPtr() + Index;
			}
			return nullptr;
		}

		// Returns a Pointer to the current Data
		CharType* GetDataPtr() const
		{
			if (m_Reserved < SmallBuffSize)
			{
				return (CharType*)m_Small;
			}
			else
			{
				return m_Data;
			}
		}

	private:
		static constexpr SizeType SmallBuffSize = 16 / sizeof(CharType) < 1 ? 1 : 16 / sizeof(CharType);

		union
		{
			CharType m_Small[SmallBuffSize] = { 0 };
			CharType* m_Data;
		};
		SizeType m_Size = 0;
		SizeType m_Reserved = SmallBuffSize - 1;
		Alloc m_Allocator;
	};

	template<>
	struct StringConverter<char, wchar_t>
	{
		template<typename Alloc1, typename Alloc2>
		static void Convert(BasicString<char, Alloc1>& To, const BasicString<wchar_t, Alloc2>& From)
		{
			size_t Length = wcstombs(nullptr, From.Data(), From.Size());
			if (Length == -1)
			{
				return;
			}

			To.Clear(Length);
			To.Append((char)0, Length);
			wcstombs(To.Data(), From.Data(), To.Size());
		}
	};

	template<>
	struct StringConverter<wchar_t, char>
	{
		template<typename Alloc1, typename Alloc2>
		static void Convert(BasicString<wchar_t, Alloc1>& To, const BasicString<char, Alloc2>& From)
		{
			size_t Length = mbstowcs(nullptr, From.Data(), From.Size());
			if (Length == -1)
			{
				return;
			}

			To.Clear(Length);
			To.Append((wchar_t)0, Length);
			mbstowcs(To.Data(), From.Data(), To.Size());
		}
	};

	typedef BasicString<char> String;
	typedef BasicString<wchar_t> Text;
}

namespace std
{
	template <class Elem, class Alloc>
	struct hash<CE::BasicString<Elem, Alloc>>
	{
		[[nodiscard]] size_t operator()(const CE::BasicString<Elem, Alloc>& _Keyval) const noexcept
		{
			return _Hash_array_representation(_Keyval.Data(), _Keyval.Size());
		}
	};
}
