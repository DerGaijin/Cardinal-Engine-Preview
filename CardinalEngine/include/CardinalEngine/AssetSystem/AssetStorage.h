#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/Reflection/Type.h"
#include "AssetID.h"
#include "Asset.h"

namespace CE
{
	struct AssetHeader 
	{
		Type AssetType;
		Array<AssetID> Dependencies;
	};

	CE_API Archive& operator<<(Archive& Ar, AssetHeader& Value);

	class CE_API AssetStorage : public Singleton
	{
		friend class AssetSystem;
		friend class AssetLoadTask;
	public:
		static bool IsSubdirectoryOf(const std::filesystem::path& Subdirectory, const std::filesystem::path& Path);

	public:
		AssetStorage(const String& ID, const std::filesystem::path& Path) : Hash(std::hash<String>()(ID)), ID(ID), Path(Path) {}

		// Discover all assets that are in the storage
		virtual void DiscoverAll() = 0;

		// Creates all directories of the path
		virtual bool CreateDirectories(const std::filesystem::path& Directories) = 0;

		// Rename a asset
		virtual bool Rename(const std::filesystem::path& Path, const String& NewName) = 0;

		// Delete a asset
		virtual bool Delete(const std::filesystem::path& Path) = 0;

	public:
		// Returns all asset headers that are discovered
		const Map<std::filesystem::path, AssetHeader>& AssetHeaders() const
		{
			return m_AssetHeaders;
		}


	protected:
		// Stores an asset to the storage
		virtual bool Store(Asset& Asset, const std::filesystem::path& Path) = 0;

		// Loads an asset from the storage
		virtual SharedPtr<Asset> Load(const std::filesystem::path& Path) = 0;

		// Calls serialize(Archive& Ar) on Asset
		static void SerializeAsset(Archive& Ar, Asset& Asset);

	public:
		const uint64 Hash = 0;
		const String ID;
		const std::filesystem::path Path;

	protected:
		Map<std::filesystem::path, AssetHeader> m_AssetHeaders;
	};
}
