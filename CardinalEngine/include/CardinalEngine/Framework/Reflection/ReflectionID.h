#pragma once
#include "../Core/Headers.h"

namespace CE
{
	// Reflection ID for Types, Methods and Properties
	class CE_API ReflectionID
	{
	public:
		// Returns the TypeName of T
		template<typename T>
		static String TypeName()
		{
			return TypeNameUnqualified<Unqualified_t<T>>();
		}

	private:
		template<typename T>
		static String TypeNameUnqualified()
		{
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
			String Name = String(__FUNCSIG__);
			Name = Name.SubString(102, Name.Size() - 109);
#endif
			return Name;
		}

	public:
		// Default Constructor
		ReflectionID() = default;

		// Construct with Hash
		ReflectionID(size_t Hash) : m_Hash(Hash) {}

		// Construct from String
		ReflectionID(const String& Str) : m_Hash(std::hash<String>()(Str)) {}

		// Creates a ReflectionID from T
		template<typename T>
		static ReflectionID Create()
		{
			return ReflectionID(TypeName<T>());
		}

	public:
		// Returns the Hash
		size_t Hash() const
		{
			return m_Hash;
		}

		// Equal Operator
		bool operator==(const ReflectionID& Other) const
		{
			return m_Hash == Other.m_Hash;
		}

		// Not Equal Operator
		bool operator!=(const ReflectionID& Other) const
		{
			return m_Hash != Other.m_Hash;
		}

		// Less Operator
		bool operator<(const ReflectionID& Other) const
		{
			return m_Hash < Other.m_Hash;
		}

	private:
		size_t m_Hash = 0;
	};
}
