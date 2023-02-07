#pragma once
#include "../Core/Headers.h"
#include "Archive.h"

#include <istream>

namespace CE
{
	class CE_API StreamArchive : public Archive
	{
	public:
		// Constructor with ostream as Write Output
		StreamArchive(std::ostream& InStream) : m_IsLoading(false), m_OStream(&InStream) {}
		// Constructor with istream as Read Input
		StreamArchive(std::istream& InStream) : m_IsLoading(true), m_IStream(&InStream) {}

		virtual bool IsLoading() const override
		{
			return m_IsLoading;
		}

		virtual Archive& operator <<(const EArchiveFlag& Flag) override;

		virtual Archive& operator <<(ArchiveNameTag& Name) override;

		virtual Archive& operator <<(ArchiveSizeTag& Size) override;

		virtual Archive& operator <<(ArchiveBinary& Binary) override;

		virtual Archive& operator <<(bool& Value) override;

		virtual Archive& operator <<(signed char& Value) override;

		virtual Archive& operator <<(signed short& Value) override;

		virtual Archive& operator <<(signed int& Value) override;

		virtual Archive& operator <<(signed long long& Value) override;

		virtual Archive& operator <<(unsigned char& Value) override;

		virtual Archive& operator <<(unsigned short& Value) override;

		virtual Archive& operator <<(unsigned int& Value) override;

		virtual Archive& operator <<(unsigned long long& Value) override;

		virtual Archive& operator <<(float& Value) override;

		virtual Archive& operator <<(double& Value) override;

		virtual Archive& operator <<(long double& Value) override;

	private:
		template<typename T>
		void Serialize_Impl(T& Value)
		{
			if (IsLoading())
			{
				m_IStream->read((char*)&Value, sizeof(Value));
			}
			else
			{
				m_OStream->write((char*)&Value, sizeof(Value));
			}

			if (m_Sizes.Size() > 0 && m_Sizes[m_Sizes.Size() - 1] > 0)
			{
				m_Sizes[m_Sizes.Size() - 1]--;
				if (m_Sizes[m_Sizes.Size() - 1] <= 0)
				{
					m_Sizes.RemoveAt(m_Sizes.Size() - 1);
				}
			}
		}

	public:
		const bool m_IsLoading;
		union
		{
			std::ostream* m_OStream;
			std::istream* m_IStream;
		};

		Array<size_t> m_Sizes;
	};
}
