#pragma once
#include "../Framework/Core/Headers.h"
#include "AssetRef.h"
#include "AssetImporter.h"
#include "AssetStorage.h"

namespace CE
{
	class CE_API AssetSystem : public Singleton
	{
	public:
		struct AssetEntry
		{
			SharedPtr<AssetRefData> Asset;
		};

	public:
		// Initializes the asset system
		bool Initialize();
	
		// Updates the asset system
		void Tick(float DeltaTime);

		// Terminates the asset system
		void Terminate();


	public:
		// Registers a asset importer
		bool AddImporter(const SharedPtr<AssetImporter>& Importer);

		// Unregisters a asset importer by name
		bool RemoveImporter(const String& Name);

		// Unregisters a asset importer by pointer
		bool RemoveImporter(AssetImporter* Importer);

		// Returns the registered asset importer with the name
		SharedPtr<AssetImporter> GetAssetImporter(const String& Name) const;

		// Returns all asset importers that can import the file extension
		const Array<SharedPtr<AssetImporter>>& GetAssetImporters(const String& Extension) const;

		// Returns all registered asset importers
		const Map<String, SharedPtr<AssetImporter>>& GetAllImporters() const
		{
			return m_Importers;
		}

		// Returns all extensions that can be imported with the supported importer
		const Map<String, Array<SharedPtr<AssetImporter>>>& GetExtensionImporters()
		{
			return m_ExtensionImporters;
		}

		// Imports a asset from file
		SharedPtr<AssetImport> Import(const SharedPtr<AssetImporter>& Importer, const std::filesystem::path& Path, bool UseWorker = true);

	public:
		// Registers a asset storage
		SharedPtr<AssetStorage> AddAssetStorage(const String& Name, const std::filesystem::path& Path);

		// Unregisters a asset storage by hash
		bool RemoveAssetStorage(uint64 StorageHash);

		// Unregisters a asset storage by name
		bool RemoveAssetStorage(const String& Name);

		// Unregisters a asset storage by path
		bool RemoveAssetStorage(const std::filesystem::path& Path);

		// Unregisters a asset storage by pointer
		bool RemoveAssetStorage(AssetStorage* Storage);

		// Unregisters a asset storage by shared pointer
		bool RemoveAssetStorage(SharedPtr<AssetStorage> Storage);

		// Returns the asset storage registered with the hash
		SharedPtr<AssetStorage> GetAssetStorage(uint64 StorageHash) const;

		// Returns the asset storage registered with the name
		SharedPtr<AssetStorage> GetAssetStorage(const String& Name) const;

		// Returns the asset storage registered with the path
		SharedPtr<AssetStorage> GetAssetStorage(const std::filesystem::path& Path) const;

		// Returns all registered asset storages
		const Map<uint64, SharedPtr<AssetStorage>>& GetAssetStorages() const
		{
			return m_AssetStorages;
		}


	public:
		// Gets a asset with ref (if not loaded it start loading it)
		template<typename A>
		bool Get(AssetRef<A>& AssetRef)
		{
			AssetRef.m_Data = GetAssetData(AssetRef.m_ID, false);
			return AssetRef.m_Data != nullptr;
		}
		
		// Returns the AssetRef with the Asset ID
		template<typename A>
		AssetRef<A> Get(const AssetID& ID)
		{
			return AssetRef<A>(ID, GetAssetData(ID, false));
		}

		// Loads the Asset with the ID
		bool Load(const AssetID& ID, bool Reload = false);

		// Stores the AssetRef
		template<typename A>
		bool Store(const AssetRef<A>& AssetRef)
		{
			return StoreAssetData(AssetRef.m_ID, AssetRef.m_Data);
		}

		// Stores the AssetRef with a new ID
		bool Store(const SharedPtr<Asset>& Asset, const AssetID& ID);

		// Returns all loaded assets
		const Map<AssetID, AssetEntry>& LoadedAssets() const
		{
			return m_LoadedAssets;
		}

	private:
		// Get the asset data for ID
		SharedPtr<AssetRefData> GetAssetData(const AssetID& ID, bool ForceLoad);

		// Store the asset data to storage
		bool StoreAssetData(const AssetID& ID, const SharedPtr<AssetRefData>& Data);

	private:
		Map<String, SharedPtr<AssetImporter>> m_Importers;
		Map<String, Array<SharedPtr<AssetImporter>>> m_ExtensionImporters;

		Map<uint64, SharedPtr<AssetStorage>> m_AssetStorages;

		Map<AssetID, AssetEntry> m_LoadedAssets;
	};
}
