#include "CardinalEngine/AssetSystem/AssetSystem.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/AssetSystem/AssetStorage_Directory.h"

namespace CE
{
	class AssetImportTask : public TaskBase
	{
	public:
		virtual void Execute() override
		{
			Importer->Import(Importing);
		}

		SharedPtr<AssetImporter> Importer;
		SharedPtr<AssetImporter::AssetImporting> Importing;
	};

	class AssetLoadTask : public TaskBase
	{
	public:
		virtual void Execute() override
		{
			SharedPtr<Asset> LoadedAsset = Storage->Load(Path);
			if (LoadedAsset != nullptr)
			{
				std::lock_guard<Spinlock> Lock(AssetData->Lock);
				AssetData->Asset = LoadedAsset;
			}
			Result.set_value(LoadedAsset != nullptr);
		}

		SharedPtr<AssetStorage> Storage;
		std::filesystem::path Path;
		SharedPtr<AssetRefData> AssetData;
		std::promise<bool> Result;
	};

	bool AssetSystem::Initialize()
	{
		return true;
	}

	void AssetSystem::Tick(float DeltaTime)
	{

	}

	void AssetSystem::Terminate()
	{

	}

	bool AssetSystem::AddImporter(const SharedPtr<AssetImporter>& Importer)
	{
		auto Found = m_Importers.find(Importer->Name());
		if (Found != m_Importers.end())
		{
			return false;
		}

		SharedPtr<AssetImporter>& It = m_Importers.emplace(Importer->Name(), Importer).first->second;

		Array<String> Extensions = It->Extensions();
		for (auto& Extension : Extensions)
		{
			m_ExtensionImporters[Extension].AddUnique(It);
		}

		return true;
	}

	bool AssetSystem::RemoveImporter(const String& Name)
	{
		auto Found = m_Importers.find(Name);
		if (Found == m_Importers.end())
		{
			return false;
		}

		Array<String> Extensions = Found->second->Extensions();
		for (auto& Extension : Extensions)
		{
			Array<SharedPtr<AssetImporter>>& Importers = m_ExtensionImporters[Extension];
			Importers.Remove(Found->second);
			if (Importers.Size() == 0)
			{
				m_ExtensionImporters.erase(Extension);
			}
		}

		m_Importers.erase(Found);

		return true;
	}

	bool AssetSystem::RemoveImporter(AssetImporter* Importer)
	{
		if (Importer != nullptr)
		{
			return RemoveImporter(Importer->Name());
		}
		return false;
	}

	SharedPtr<AssetImporter> AssetSystem::GetAssetImporter(const String& Name) const
	{
		auto Found = m_Importers.find(Name);
		if (Found != m_Importers.end())
		{
			return Found->second;
		}
		return nullptr;
	}

	const Array<SharedPtr<AssetImporter>>& AssetSystem::GetAssetImporters(const String& Extension) const
	{
		auto Found = m_ExtensionImporters.find(Extension);
		if (Found != m_ExtensionImporters.end())
		{
			return Found->second;
		}
		static Array<SharedPtr<AssetImporter>> Empty;
		return Empty;
	}

	SharedPtr<AssetImport> AssetSystem::Import(const SharedPtr<AssetImporter>& Importer, const std::filesystem::path& Path, bool UseWorker /*= true*/)
	{
		SharedPtr<AssetImportTask> ImportTask = SharedPtr<AssetImportTask>(new AssetImportTask());
		ImportTask->Importer = Importer;
		ImportTask->Importing = SharedPtr<AssetImporter::AssetImporting>(new AssetImporter::AssetImporting());
		ImportTask->Importing->Path = Path;
		ImportTask->Importing->Result = SharedPtr<AssetImport>(new AssetImport());
		ImportTask->Importing->Result->State = ImportTask->Importing->State.get_future().share();
		if (UseWorker)
		{
			Engine::Get().TaskSystem.Submit(EThreadType::Worker, ImportTask);
		}
		else
		{
			ImportTask->Execute();
		}

		return ImportTask->Importing->Result;
	}

	SharedPtr<AssetStorage> AssetSystem::AddAssetStorage(const String& Name, const std::filesystem::path& Path)
	{
		std::filesystem::path AbsoluteStoragePath = std::filesystem::absolute(Path).lexically_normal();

		uint64 StorageHash = std::hash<String>()(Name);
		auto FoundID = m_AssetStorages.find(StorageHash);
		if (FoundID != m_AssetStorages.end())
		{
			return FoundID->second->Path == AbsoluteStoragePath ? FoundID->second : nullptr;
		}

		auto FoundPath = std::find_if(m_AssetStorages.begin(), m_AssetStorages.end(), [&](const std::pair<const uint64, SharedPtr<AssetStorage>>& It) { return It.second->Path == AbsoluteStoragePath; });
		if (FoundPath != m_AssetStorages.end())
		{
			return FoundPath->second;
		}

		SharedPtr<AssetStorage> Storage;
		if (std::filesystem::is_directory(AbsoluteStoragePath))
		{
			SharedPtr<AssetStorage_Directory> ASD = SharedPtr<AssetStorage_Directory>(new AssetStorage_Directory(Name, AbsoluteStoragePath));
			//Application::Get().RegisterFileWatcher(ASD);
			Storage = ASD;
		}
		else if (std::filesystem::is_regular_file(AbsoluteStoragePath) && AbsoluteStoragePath.extension() == ".CEAS")
		{
			//Storage = UniquePtr<AssetStorage>(new AssetStorage_Archive(AbsoluteStoragePath, StorageName));
			return nullptr;
		}
		else
		{
			return nullptr;
		}

		return m_AssetStorages.emplace(StorageHash, Storage).first->second;
	}

	bool AssetSystem::RemoveAssetStorage(uint64 StorageHash)
	{
		auto Found = m_AssetStorages.find(StorageHash);
		if (Found == m_AssetStorages.end())
		{
			return false;
		}
		SharedPtr<AssetStorage_Directory> ASD = SharedPtr<AssetStorage_Directory>::DynamicCast(Found->second);
		if (ASD != nullptr)
		{
			//Application::Get().UnregisterFileWatcher(ASD);
		}
		m_AssetStorages.erase(Found);
		return true;
	}

	bool AssetSystem::RemoveAssetStorage(const String& Name)
	{
		return RemoveAssetStorage(std::hash<String>()(Name));
	}

	bool AssetSystem::RemoveAssetStorage(const std::filesystem::path& Path)
	{
		auto Found = std::find_if(m_AssetStorages.begin(), m_AssetStorages.end(), [&](const std::pair<const uint64, SharedPtr<AssetStorage>>& It)
			{
				return It.second->Path == Path;
			});
		if (Found == m_AssetStorages.end())
		{
			return false;
		}
		SharedPtr<AssetStorage_Directory> ASD = SharedPtr<AssetStorage_Directory>::DynamicCast(Found->second);
		if (ASD != nullptr)
		{
			//Application::Get().UnregisterFileWatcher(ASD);
		}
		m_AssetStorages.erase(Found);
		return true;
	}

	bool AssetSystem::RemoveAssetStorage(AssetStorage* Storage)
	{
		return Storage != nullptr ? RemoveAssetStorage(Storage->Hash) : false;
	}

	bool AssetSystem::RemoveAssetStorage(SharedPtr<AssetStorage> Storage)
	{
		return RemoveAssetStorage(Storage.Get());
	}

	SharedPtr<AssetStorage> AssetSystem::GetAssetStorage(uint64 StorageHash) const
	{
		auto Found = m_AssetStorages.find(StorageHash);
		if (Found != m_AssetStorages.end())
		{
			return Found->second;
		}
		return nullptr;
	}

	SharedPtr<AssetStorage> AssetSystem::GetAssetStorage(const String& Name) const
	{
		return GetAssetStorage(std::hash<String>()(Name));
	}

	SharedPtr<AssetStorage> AssetSystem::GetAssetStorage(const std::filesystem::path& Path) const
	{
		auto Found = std::find_if(m_AssetStorages.begin(), m_AssetStorages.end(), [&](const std::pair<const uint64, SharedPtr<AssetStorage>>& It)
			{
				return It.second->Path == Path;
			});
		if (Found != m_AssetStorages.end())
		{
			return Found->second;
		}
		return nullptr;
	}

	bool AssetSystem::Load(const AssetID& ID, bool Reload /*= false*/)
	{
		return GetAssetData(ID, Reload) != nullptr;
	}

	bool AssetSystem::Store(const SharedPtr<Asset>& Asset, const AssetID& ID)
	{
		SharedPtr<AssetRefData> Tmp = AssetRefData::Create(Asset);
		return StoreAssetData(ID, Tmp);
	}

	SharedPtr<AssetRefData> AssetSystem::GetAssetData(const AssetID& ID, bool ForceLoad)
	{
		if (ID.StorageHash() == 0 || ID.Path().empty())
		{
			return nullptr;
		}

		auto LoadedAsset = m_LoadedAssets.find(ID);
		if (LoadedAsset != m_LoadedAssets.end())
		{
			if (!ForceLoad)
			{
				return LoadedAsset->second.Asset;
			}
		}
		else
		{
			LoadedAsset = m_LoadedAssets.emplace(ID, AssetEntry{ SharedPtr<AssetRefData>(new AssetRefData()) }).first;
		}

		auto FoundStorage = m_AssetStorages.find(ID.StorageHash());
		if (FoundStorage == m_AssetStorages.end())
		{
			return nullptr;
		}

		SharedPtr<AssetLoadTask> LoadTask = SharedPtr<AssetLoadTask>(new AssetLoadTask());
		LoadTask->Storage = FoundStorage->second;
		LoadTask->Path = ID.Path();
		LoadTask->AssetData = LoadedAsset->second.Asset;
		{
			std::lock_guard<Spinlock> Lock(LoadTask->AssetData->Lock);
			LoadTask->AssetData->LoadState = LoadTask->Result.get_future().share();
		}

		Engine::Get().TaskSystem.Submit(EThreadType::Worker, LoadTask);

		return LoadTask->AssetData;
	}

	bool AssetSystem::StoreAssetData(const AssetID& ID, const SharedPtr<AssetRefData>& Data)
	{
		if (ID.StorageHash() == 0 || ID.Path().empty() || Data == nullptr || Data->Asset == nullptr)
		{
			return false;
		}

		auto FoundStorage = m_AssetStorages.find(ID.StorageHash());
		if (FoundStorage == m_AssetStorages.end())
		{
			return false;
		}

		return FoundStorage->second->Store(*Data->Asset, ID.Path());
	}
}
