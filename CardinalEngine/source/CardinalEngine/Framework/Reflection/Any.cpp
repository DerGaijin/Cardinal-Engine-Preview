#include "CardinalEngine/Framework/Reflection/Any.h"

namespace CE
{
	Any& Any::operator=(const Any& Other)
	{
		Clear();
		if (Other.IsCopyable())
		{
			Other.m_Copy(m_Data, Other.m_Data);
			m_Type = Other.m_Type;
			m_Copy = Other.m_Copy;
			m_Move = Other.m_Move;
			m_Delete = Other.m_Delete;
		}
		return *this;
	}

	Any& Any::operator=(Any&& Other) noexcept
	{
		Clear();

		// Try to Move the Other
		if (Other.IsMoveable())
		{
			Other.m_Move(m_Data, Other.m_Data);
			m_Type = Other.m_Type;
			m_Copy = Other.m_Copy;
			m_Move = Other.m_Move;
			m_Delete = Other.m_Delete;
			Other.Clear();
		}
		// If Other is not Movable we copy it
		else if (Other.IsCopyable())
		{
			Other.m_Copy(m_Data, Other.m_Data);
			m_Type = Other.m_Type;
			m_Copy = Other.m_Copy;
			m_Move = Other.m_Move;
			m_Delete = Other.m_Delete;
			Other.Clear();
		}
		return *this;
	}

	void Any::Clear()
	{
		if (m_Data != nullptr && m_Delete != nullptr)
		{
			m_Delete(m_Data);
		}

		m_Data = nullptr;
		m_Type = CE::Type();
		m_Copy = nullptr;
		m_Move = nullptr;
		m_Delete = nullptr;
	}
}
