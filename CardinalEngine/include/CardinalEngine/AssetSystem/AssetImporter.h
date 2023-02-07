#pragma once
#include "../Framework/Core/Headers.h"
#include "Asset.h"

namespace CE
{
	enum class EAssetImportResult
	{
		Success,
		Failed,
		FileNotFound,
	};

	struct AssetImport 
	{
		std::shared_future<EAssetImportResult> State;
		Array<SharedPtr<Asset>> Assets;
	};

	class CE_API AssetImporter
	{
	public:
		struct AssetImporting 
		{
			std::promise<EAssetImportResult> State;
			SharedPtr<AssetImport> Result;
			std::filesystem::path Path;
		};

	public:

		// Returns the name of the importer
		virtual String Name() const = 0;
	
		// Returns an array of extensions that this importer can import
		virtual Array<String> Extensions() const = 0;
		
		// Imports files from disk
		virtual void Import(const SharedPtr<AssetImporting>& Import) = 0;

	};
}
