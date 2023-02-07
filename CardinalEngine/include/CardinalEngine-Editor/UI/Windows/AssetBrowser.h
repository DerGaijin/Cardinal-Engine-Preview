#pragma once
#include "../../../CardinalEngine/Framework/Core/Headers.h"
#include "../EditorWindow.h"
#include "CardinalEngine/AssetSystem/AssetRef.h"
#include "CardinalEngine/Material/Texture.h"

namespace CE
{
	class AssetStorage;
	class AssetHeader;
	class AssetImporter;

	class CE_API AssetBrowser : public EditorWindow
	{
	public:
		enum class EDisplayType
		{
			Tiles,
			List,
		};

		enum class ETileSize
		{
			Tiny,
			Small,
			Normal,
			Big,
			Large,
		};

	public:
		AssetBrowser();

		virtual void Draw(float DeltaTime) override;

	private:
		void DrawHierarchyHeader();
		void DrawHierarchy();
		void DrawHierarchyDirectory(const SharedPtr<AssetStorage>& AS, bool IsStorageSelected, const Map<std::filesystem::path, AssetHeader>& Headers, Map<std::filesystem::path, AssetHeader>::const_iterator& It, const std::filesystem::path& Path);
		void DrawDirectoryHeader();
		void DrawDirectory();
		void DrawDirectoryItem(const AssetHeader& Header, const std::filesystem::path& Path, uint32 Index);

		void DrawImportPopup();

	public:
		WeakPtr<AssetStorage> SelectedStorage;
		std::filesystem::path SelectedPath;

		EDisplayType DisplayType = EDisplayType::Tiles;
		ETileSize TileSize = ETileSize::Normal;

		AssetRef<Texture> Icon_Unknown;
		AssetRef<Texture> Icon_Directory;
		Map<Type, AssetRef<Texture>> Icon_Asset;

	private:
		void RefreshAllStorages();
		void OpenAssetEditor(const SharedPtr<AssetStorage>& Storage, const std::filesystem::path& Path, const AssetHeader& Header);

	private:
		String m_Search;

		Set<std::filesystem::path> m_Selected;
		
		bool m_Rename = false;
		String m_NewName;

		struct ImportSettings 
		{
			std::filesystem::path Path;
			WeakPtr<AssetImporter> Importer;
		};
		Array<ImportSettings> m_Imports;
	};
}
