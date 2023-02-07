#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"
#include "CardinalEngine/Framework/Reflection/ReflectionID.h"

namespace CE
{
	Archive& operator<<(Archive& Ar, Transform& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("Location") << Value.Location;
		Ar << ArchiveNameTag("Rotation") << Value.Rotation;
		Ar << ArchiveNameTag("Scale") << Value.Scale;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	Archive& operator<<(Archive& Ar, Color& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("R") << Value.R;
		Ar << ArchiveNameTag("G") << Value.G;
		Ar << ArchiveNameTag("B") << Value.B;
		Ar << ArchiveNameTag("A") << Value.A;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	Archive& operator<<(Archive& Ar, VersionNumber& Value)
	{
		Ar << EArchiveFlag::BeginObject;
		Ar << ArchiveNameTag("Major") << (signed char&)Value.Major;
		Ar << ArchiveNameTag("Minor") << Value.Minor;
		Ar << ArchiveNameTag("Patch") << (signed char&)Value.Patch;
		Ar << EArchiveFlag::EndObject;
		return Ar;
	}

	Archive& operator<<(Archive& Ar, std::filesystem::path& Value)
	{
		std::wstring WideStr = Value.wstring();
		Ar << WideStr;
		if (Ar.IsLoading())
		{
			Value = WideStr;
		}
		return Ar;
	}

	CE_API Archive& operator<<(Archive& Ar, ReflectionID& Value)
	{
		uint64_t Hash = Value.Hash();
		Ar << Hash;
		if (Ar.IsLoading())
		{
			Value = ReflectionID(Hash);
		}
		return Ar;
	}
}
