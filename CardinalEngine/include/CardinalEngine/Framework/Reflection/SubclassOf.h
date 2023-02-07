#pragma once
#include "../Core/Headers.h"
#include "Type.h"

namespace CE
{
	// Allows only the Class and Subclasses of it
	template<typename T>
	class SubclassOf
	{
		static_assert(std::is_class_v<T>, "T is not a Class");

	public:
		// Default constructor
		SubclassOf() = default;
	
		// Copy Constructor
		SubclassOf(const SubclassOf&) = default;

		// Move Constructor
		SubclassOf(SubclassOf&&) = default;

		// Copy Operator
		SubclassOf& operator=(const SubclassOf&) = default;

		// Move Operator
		SubclassOf& operator=(SubclassOf&&) = default;

		// Construct with Type
		SubclassOf(const Type& Type)
		{
			*this = std::forward<const CE::Type&>(Type);
		}

		// Assign Type
		SubclassOf& operator=(const Type& Type) 
		{
			CE::Type TType = Type::Get<T>();
			if (TType == Type || TType.IsBaseOf(Type))
			{
				m_Type = Type;
			}
			return *this;
		}

		// Cast to Type
		operator const Type&() const
		{
			return m_Type;
		}

		// Arrow Operator to Type
		Type& operator->()
		{
			return m_Type;
		}

		// Arrow Operator to Type
		const Type& operator->() const
		{
			return m_Type;
		}

	private:
		Type m_Type;
	};
}
