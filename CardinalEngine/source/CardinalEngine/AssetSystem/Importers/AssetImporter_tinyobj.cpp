#include "CardinalEngine/AssetSystem/Importers/AssetImporter_tinyobj.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace CE
{
	Array<String> AssetImporter_tinyobj::Extensions() const
	{
		return { "obj" };
	}

	void AssetImporter_tinyobj::Import(const SharedPtr<AssetImporting>& Import)
	{
		Import->State.set_value(EAssetImportResult::Failed);
	}
}
