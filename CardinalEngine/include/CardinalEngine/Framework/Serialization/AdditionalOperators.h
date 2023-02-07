#pragma once
#include "../Core/Headers.h"
#include "Archive.h"

namespace CE
{
	class ReflectionID;

	template<typename Elem, typename Alloc = std::allocator<Elem>>
	Archive& operator<<(Archive& Ar, Array<Elem, Alloc>& Value)
	{
		size_t Size = Value.Size();
		Ar << ArchiveSizeTag(Size);
		if (Ar.IsLoading())
		{
			Value.Clear(Size);
			if (Size > 0)
			{
				Value.AddDefaulted(Size);
			}
		}
		for (auto& It : Value)
		{
			Ar << It;
		}
		return Ar;
	}

	//@TODO: Map

	//@TODO: Unordered Map

	//@TODO: MultiMap

	//@TODO: Unordered MultiMap

	//@TODO: Set

	//@TODO: Unordered Set

	template<typename Elem, typename Alloc = std::allocator<Elem>>
	Archive& operator<<(Archive& Ar, BasicString<Elem, Alloc>& Value)
	{
		size_t BinarySize = Value.Size() * sizeof(Elem);
		Ar << ArchiveSizeTag(BinarySize);
		size_t Size = BinarySize / sizeof(Elem);
		if (Ar.IsLoading())
		{
			Value.Clear(Size);
			Value.Append((Elem)0, Size);
		}
		Ar << ArchiveBinary((char*)Value.Data());
		return Ar;
	}

	template<typename Elem, typename Alloc = std::allocator<Elem>>
	Archive& operator<<(Archive& Ar, std::basic_string<Elem, Alloc>& Value)
	{
		size_t BinarySize = Value.size() * sizeof(Elem);
		Ar << ArchiveSizeTag(BinarySize);
		size_t Size = BinarySize / sizeof(Elem);
		if (Ar.IsLoading())
		{
			Value.resize(Size);
		}
		Ar << ArchiveBinary((char*)Value.data());
		return Ar;
	}

	template<typename T>
	Archive& operator<<(Archive& Ar, Mat4x4<T>& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("Row0") << Value.Row0;
		Ar << ArchiveNameTag("Row1") << Value.Row1;
		Ar << ArchiveNameTag("Row2") << Value.Row2;
		Ar << ArchiveNameTag("Row3") << Value.Row3;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	template<typename T>
	Archive& operator<<(Archive& Ar, Vec2<T>& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("X") << Value.X;
		Ar << ArchiveNameTag("Y") << Value.Y;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	template<typename T>
	Archive& operator<<(Archive& Ar, Vec3<T>& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("X") << Value.X;
		Ar << ArchiveNameTag("Y") << Value.Y;
		Ar << ArchiveNameTag("Z") << Value.Z;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	template<typename T>
	Archive& operator<<(Archive& Ar, Vec4<T>& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("X") << Value.X;
		Ar << ArchiveNameTag("Y") << Value.Y;
		Ar << ArchiveNameTag("Z") << Value.Z;
		Ar << ArchiveNameTag("W") << Value.W;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	template<typename T>
	Archive& operator<<(Archive& Ar, Quat<T>& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("X") << Value.X;
		Ar << ArchiveNameTag("Y") << Value.Y;
		Ar << ArchiveNameTag("Z") << Value.Z;
		Ar << ArchiveNameTag("W") << Value.W;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	CE_API Archive& operator<<(Archive& Ar, Transform& Value);

	CE_API Archive& operator<<(Archive& Ar, Color& Value);

	CE_API Archive& operator<<(Archive& Ar, VersionNumber& Value);

	CE_API Archive& operator<<(Archive& Ar, std::filesystem::path& Value);

	CE_API Archive& operator<<(Archive& Ar, ReflectionID& Value);
}
