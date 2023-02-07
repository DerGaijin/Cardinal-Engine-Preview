#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class CE_API AssetID
	{
	public:
		AssetID() = default;

		AssetID(const String& Storage, std::filesystem::path Path) : m_StorageHash(std::hash<String>()(Storage)), m_Path(Path.replace_extension()) {}
	
		AssetID(uint64 StorageHash, std::filesystem::path Path) : m_StorageHash(StorageHash), m_Path(Path.replace_extension()) {}

		AssetID(const AssetID&) = default;
		
		AssetID(AssetID&&) = default;
		
		AssetID& operator=(const AssetID&) = default;
		
		AssetID& operator=(AssetID&&) = default;

		bool operator==(const AssetID& Other) const
		{
			return m_StorageHash == Other.m_StorageHash && m_Path == Other.m_Path;
		}

		bool operator!=(const AssetID& Other) const
		{
			return !(*this == Other);
		}

		bool operator<(const AssetID& Other) const
		{
			return m_StorageHash == Other.m_StorageHash ? m_Path < Other.m_Path : m_StorageHash < Other.m_StorageHash;
		}

	public:
		const uint64& StorageHash() const
		{
			return m_StorageHash;
		}

		const std::filesystem::path& Path() const
		{
			return m_Path;
		}

	private:
		uint64 m_StorageHash = 0;
		std::filesystem::path m_Path;
	};

	class Archive;
	CE_API Archive& operator<<(Archive& Ar, AssetID& Value);
}
