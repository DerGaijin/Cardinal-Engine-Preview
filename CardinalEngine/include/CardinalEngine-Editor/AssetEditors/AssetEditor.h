#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../UI/EditorWindow.h"

namespace CE
{
	class CE_API AssetEditor : public EditorWindow
	{
	public:
		AssetEditor(size_t StorageHash, const std::filesystem::path& Path) : m_StorageHash(StorageHash), m_Path(Path) {}
	
		virtual bool RemoveOnClosed() override
		{
			return true;
		}

		String GetSuffix() const;

	public:
		size_t StorageHash() const
		{
			return m_StorageHash;
		}

		const std::filesystem::path& Path() const
		{
			return m_Path;
		}

	private:
		size_t m_StorageHash = 0;
		std::filesystem::path m_Path;
	};
}
