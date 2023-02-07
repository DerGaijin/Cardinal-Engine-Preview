#pragma once
#include "../Framework/Core/Headers.h"
#include "AssetStorage.h"

namespace CE
{
	class CE_API AssetStorage_Directory : public AssetStorage, public SharedFromThis<AssetStorage_Directory>
	{
	public:
		AssetStorage_Directory(const String& ID, const std::filesystem::path& Path);

		virtual void DiscoverAll() override;

		virtual bool CreateDirectories(const std::filesystem::path& Directories) override;

		virtual bool Rename(const std::filesystem::path& Path, const String& NewName) override;

		virtual bool Delete(const std::filesystem::path& Path) override;

	protected:
		virtual bool Store(Asset& Asset, const std::filesystem::path& Path) override;

		virtual SharedPtr<Asset> Load(const std::filesystem::path& Path) override;
	};
}
