#pragma once
#include "../../Framework/Core/Headers.h"
#include "../AssetImporter.h"

namespace CE
{
	// Asset importer for .obj files
	class CE_API AssetImporter_tinyobj : public AssetImporter
	{
	public:
		virtual String Name() const override
		{
			return "tiny_obj_loader";
		}

		virtual Array<String> Extensions() const override;

		virtual void Import(const SharedPtr<AssetImporting>& Import) override;

	};
}
