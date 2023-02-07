#include "CardinalEngine/AssetSystem/AssetID.h"
#include "CardinalEngine/Framework/Serialization/Archive.h"
#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"

namespace CE
{
	Archive& operator<<(Archive& Ar, AssetID& Value)
	{
		uint64 StorageHash = Value.StorageHash();
		std::filesystem::path Path = Value.Path();
		Ar << StorageHash;
		Ar << Path;
		if (Ar.IsLoading())
		{
			Value = AssetID(StorageHash, Path);
		}
		return Ar;
	}
}
