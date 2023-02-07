#include "CardinalEngine/Framework/Serialization/StreamArchive.h"

namespace CE
{
	Archive& StreamArchive::operator<<(const EArchiveFlag& Flag)
	{
		switch (Flag)
		{
		default:
		case CE::EArchiveFlag::None:
			break;
		case CE::EArchiveFlag::BeginObject:
		{
			m_Sizes.Emplace();
			break;
		}
		case CE::EArchiveFlag::EndObject:
		{
			if (m_Sizes.Size() > 0)
			{
				m_Sizes.RemoveAt(m_Sizes.Size() - 1);
			}
			break;
		}
		}
		return *this;
	}

	Archive& StreamArchive::operator<<(ArchiveNameTag& Name)
	{
		// Names are Ignored in Stream Archive
		return *this;
	}

	Archive& StreamArchive::operator<<(ArchiveSizeTag& Size)
	{
		*this << Size.Size;
		m_Sizes.Emplace(Size.Size);
		return *this;
	}

	Archive& StreamArchive::operator<<(ArchiveBinary& Binary)
	{
		CE_ASSERT(m_Sizes.Size() > 0, "m_Sizes is empty");
		if (IsLoading())
		{
			m_IStream->read(Binary.Data, m_Sizes[m_Sizes.Size() - 1]);
		}
		else
		{
			m_OStream->write(Binary.Data, m_Sizes[m_Sizes.Size() - 1]);
		}
		m_Sizes.RemoveAt(m_Sizes.Size() - 1);
		return *this;
	}

	Archive& StreamArchive::operator<<(bool& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(signed char& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(signed short& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(signed int& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(signed long long& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(unsigned char& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(unsigned short& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(unsigned int& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(unsigned long long& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(float& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(double& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}

	Archive& StreamArchive::operator<<(long double& Value)
	{
		Serialize_Impl(Value);
		return *this;
	}
}
