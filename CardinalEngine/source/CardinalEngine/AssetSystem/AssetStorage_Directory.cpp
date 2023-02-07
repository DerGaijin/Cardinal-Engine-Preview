#include "CardinalEngine/AssetSystem/AssetStorage_Directory.h"
#include "CardinalEngine/AssetSystem/AssetRef.h"
#include "CardinalEngine/Framework/Serialization/StreamArchive.h"
#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"
#include "CardinalEngine/Framework/Reflection/Type.h"
#include "CardinalEngine/Framework/Reflection/Any.h"
#include "CardinalEngine/Framework/Reflection/Argument.h"

namespace CE
{
	AssetStorage_Directory::AssetStorage_Directory(const String& ID, const std::filesystem::path& Path) : AssetStorage(ID, Path)
	{
		DiscoverAll();
	}

	void AssetStorage_Directory::DiscoverAll()
	{
		m_AssetHeaders.clear();

		Type VoidType = Type::Get<void>();
		for (auto& It : std::filesystem::recursive_directory_iterator(Path))
		{
			if (It.is_directory() || (It.is_regular_file() && It.path().extension() == ".CEA"))
			{
				std::filesystem::path RelativePath = std::filesystem::relative(It.path(), Path);

				if (It.is_directory())
				{
					m_AssetHeaders[RelativePath] = AssetHeader{ VoidType };
				}
				else
				{
					std::ifstream AssetFile = std::ifstream(It.path(), std::ios::binary);
					if (AssetFile.is_open())
					{
						char Prefix[3] = { 0 };
						AssetFile.read(Prefix, 3);
						if (Prefix[0] != 'C' || Prefix[1] != 'E' || Prefix[2] != 'A')
						{
							AssetFile.close();
							continue;
						}

						StreamArchive Ar(AssetFile);
						AssetHeader& Header = m_AssetHeaders[RelativePath];
						Ar << Header;
						AssetFile.close();
					}
				}
			}
		}
	}

	bool AssetStorage_Directory::CreateDirectories(const std::filesystem::path& Directories)
	{
		return std::filesystem::create_directories(Path / Directories);
	}

	bool AssetStorage_Directory::Rename(const std::filesystem::path& InPath, const String& NewName)
	{
		std::filesystem::path NewPath = Path / InPath.parent_path() / NewName.Data();
		NewPath.replace_extension(InPath.extension());
		std::filesystem::rename(Path / InPath, NewPath);
		return true;
	}

	bool AssetStorage_Directory::Delete(const std::filesystem::path& InPath)
	{
		return std::filesystem::remove_all(Path / InPath) > 0;
	}

	bool AssetStorage_Directory::Store(Asset& Asset, const std::filesystem::path& Path)
	{
		if (!Asset.GetType().IsValid())
		{
			return false;
		}

		std::filesystem::path FinalPath = this->Path;
		FinalPath += "/";
		FinalPath += Path;
		FinalPath.replace_extension(".CEA");
		FinalPath = FinalPath.lexically_normal();

		std::filesystem::create_directories(FinalPath.parent_path());

		std::ofstream AssetFile = std::ofstream(FinalPath, std::ios::binary);
		if (!AssetFile.is_open())
		{
			return false;
		}

		AssetFile.write("CEA", 3);

		AssetHeader Header;
		Header.AssetType = Asset.GetType();
		Header.Dependencies;	//@TODO: Collect Dependencies

		StreamArchive Ar(AssetFile);

		Ar << Header;
		SerializeAsset(Ar, Asset);

		AssetFile.close();

		return true;
	}

	SharedPtr<Asset> AssetStorage_Directory::Load(const std::filesystem::path& Path)
	{
		std::filesystem::path FinalPath = this->Path;
		FinalPath += "/";
		FinalPath += Path;
		FinalPath.replace_extension(".CEA");
		FinalPath = FinalPath.lexically_normal();

		std::ifstream AssetFile = std::ifstream(FinalPath, std::ios::binary);
		if (!AssetFile.is_open())
		{
			return nullptr;
		}

		char Prefix[3] = { 0 };
		AssetFile.read(Prefix, 3);
		if (Prefix[0] != 'C' || Prefix[1] != 'E' || Prefix[2] != 'A')
		{
			AssetFile.close();
			return nullptr;
		}

		StreamArchive Ar(AssetFile);

		AssetHeader Header;
		Ar << Header;

		if (!Header.AssetType.IsValid())
		{
			AssetFile.close();
			return nullptr;
		}

		SharedPtr<Asset> Asset = SharedPtr<CE::Asset>(Header.AssetType.Construct({}).ExtractValue<CE::Asset>());
		if (Asset == nullptr)
		{
			return nullptr;
		}

		//@TODO: Start Load Dependencies

		SerializeAsset(Ar, *Asset);

		AssetFile.close();
		return Asset;
	}

	/*
	void AssetStorage_Directory::OnAdded(const std::filesystem::path& Added)
	{
		RefreshAllAssets();
	}

	void AssetStorage_Directory::OnRemoved(const std::filesystem::path& Removed)
	{
		std::filesystem::path Path = Removed;
		Path.replace_extension("");

		uint32 RemoveCount = 0;
		auto It = m_AssetHeaders.find(Removed);
		while (It != m_AssetHeaders.end())
		{
			RemoveCount += 1;
			It = m_AssetHeaders.erase(It);

			if (!AssetStorage::IsSubdirectoryOf(It->first, Path))
			{
				break;
			}
		}
	}

	void AssetStorage_Directory::OnChanged(const std::filesystem::path& Changed)
	{
		RefreshAllAssets();
	}

	void AssetStorage_Directory::OnRenamed(const std::filesystem::path& Old, const std::filesystem::path& New)
	{
		RefreshAllAssets();
	}
	*/
}
