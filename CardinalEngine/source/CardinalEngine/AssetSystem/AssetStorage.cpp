#include "CardinalEngine/AssetSystem/AssetStorage.h"
#include "CardinalEngine/Framework/Serialization/Archive.h"
#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"
#include "CardinalEngine/Framework/Reflection/ReflectionID.h"

namespace CE
{
	bool AssetStorage::IsSubdirectoryOf(const std::filesystem::path& Subdirectory, const std::filesystem::path& Path)
	{
		Text SubdirectoryStr = Subdirectory.wstring();
		Text PathStr = Path.wstring();
		return SubdirectoryStr.StartsWith(PathStr);
	}

	void AssetStorage::SerializeAsset(Archive& Ar, Asset& Asset)
	{
		Asset.SerializeAsset(Ar);
	}

	Archive& operator<<(Archive& Ar, AssetHeader& Value)
	{
		ReflectionID AssetTypeID = Value.AssetType.ID();
		Ar << AssetTypeID;
		if (Ar.IsLoading())
		{
			Value.AssetType = Type::Find(AssetTypeID);
		}
		Ar << Value.Dependencies;

		return Ar;
	}
}
