#pragma once
#include "../../Framework/Core/Headers.h"
#include "../AssetImporter.h"

namespace CE
{
	// Asset importer for images
	class CE_API AssetImporter_stbi : public AssetImporter
	{
	public:
		virtual String Name() const override
		{
			return "stb_image";
		}

		virtual Array<String> Extensions() const override;

		virtual void Import(const SharedPtr<AssetImporting>& Import) override;

	};
}
