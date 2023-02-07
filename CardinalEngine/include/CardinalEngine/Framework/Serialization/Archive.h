#pragma once
#include "../Core/Headers.h"
#include <type_traits>

namespace CE
{
	enum class EArchiveFlag
	{
		None,
		BeginObject,
		EndObject,
	};


	class ArchiveNameTag
	{
	public:
		ArchiveNameTag(const char*) : Name(Name) {}
		const char* const Name;
	};


	class ArchiveSizeTag
	{
	public:
		ArchiveSizeTag(size_t& SizeRef) : Size(SizeRef) {}
		size_t& Size;
	};


	class ArchiveBinary
	{
	public:
		ArchiveBinary(char* DataPtr) : Data(DataPtr) {}
		char* Data;
	};


	class CE_API Archive
	{
	public:
		// Destructor
		virtual ~Archive() = default;

		// Returns true if the Archive is a Loading Archive
		virtual bool IsLoading() const = 0;

		// Returns true if the Archive is a Saving Archive
		bool IsSaving() const { return !IsLoading(); }

		// Inputs a Flag
		virtual Archive& operator<<(const EArchiveFlag& Flag) { return *this; }
		// Adds a Name Tag
		virtual Archive& operator<<(ArchiveNameTag& Name) = 0;
		// Inputs a Size Tag
		virtual Archive& operator<<(ArchiveSizeTag& Size) = 0;
		// Serializes Binary Data
		virtual Archive& operator<<(ArchiveBinary& Binary) = 0;


		// Serializes a Bool
		virtual Archive& operator<<(bool& Value) = 0;

		// Serializes a signed char
		virtual Archive& operator<<(signed char& Value) = 0;
		// Serializes a signed short
		virtual Archive& operator<<(signed short& Value) = 0;
		// Serializes a signed int
		virtual Archive& operator<<(signed int& Value) = 0;
		// Serializes a signed long long
		virtual Archive& operator<<(signed long long& Value) = 0;

		// Serializes a unsigned char
		virtual Archive& operator<<(unsigned char& Value) = 0;
		// Serializes a unsigned short
		virtual Archive& operator<<(unsigned short& Value) = 0;
		// Serializes a unsigned int
		virtual Archive& operator<<(unsigned int& Value) = 0;
		// Serializes a unsigned long long
		virtual Archive& operator<<(unsigned long long& Value) = 0;

		// Serializes a float
		virtual Archive& operator<<(float& Value) = 0;
		// Serializes a double
		virtual Archive& operator<<(double& Value) = 0;
		// Serializes a long double
		virtual Archive& operator<<(long double& Value) = 0;
	};

	namespace Detail_Serialization
	{
		template<typename T>
		struct HasFunction_Serialize
		{
		private:
			typedef char YesType[1];
			typedef char NoType[2];
			template <typename U, void (U::*)(Archive&)>
			class check { };
			template <typename C>
			static YesType& f(check<C, &C::Serialize>*);
			template <typename C>
			static NoType& f(...);
		public:
			using type = std::conditional_t<std::is_same_v<decltype(f<typename std::remove_cv_t<std::remove_reference_t<T>>>(0)), YesType&>, std::true_type, std::false_type>;
		};
	}

	// Serializes any class that has a Serialize(Archive& Ar) function
	template<typename T>
	std::enable_if_t<Detail_Serialization::HasFunction_Serialize<T>::type::value, Archive&> operator<<(Archive& Ar, T& Value)
	{
		Value.Serialize(Ar);
		return Ar;
	}


	// Serializes any Enum
	template<typename E>
	std::enable_if_t<std::is_enum_v<E>, Archive&> operator<<(Archive& Ar, E& Value)
	{
		return Ar << (std::underlying_type_t<ELog>&)Value;
	}
}
