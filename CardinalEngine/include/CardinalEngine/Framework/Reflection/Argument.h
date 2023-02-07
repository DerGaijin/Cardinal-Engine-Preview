#pragma once
#include "../Core/Headers.h"
#include "Type.h"
#include "Any.h"


namespace CE
{
	// Contains a Reference to Value to be used as an Argument
	class CE_API Argument
	{
	public:
		// Constructs a Argument from Any Instance
		Argument(const Any& Any)
		{
			m_Data = Any.m_Data;
			m_Type = Any.m_Type;
		}

		// Constructs a Argument from Any Object
		template<typename T, typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, Any>>>
		Argument(T&& In)
		{
			Emplace(In);
		}

		// Copy Constructor
		Argument(const Argument& O) = default;

		// Move Constructor
		Argument(Argument&& O) = default;

		// Copy operator
		Argument& operator=(const Argument& O) = delete;

		// Move operator
		Argument& operator=(Argument&& O) = delete;

		// Default Destructor
		~Argument() = default;

	public:
		// Returns true if the Argument contains a Valid value
		bool IsValid() const
		{
			return m_Data != nullptr && m_Type.IsValid();
		}

		// Returns the Type of the Value
		const Type& Type() const
		{
			return m_Type;
		}

		// Returns true if the Argument can be converted to Type
		bool IsSuitable(const CE::Type& Type) const
		{
			if (m_Type == Type)
			{
				return true;
			}

			ETypeCategory Category = Type.Category();
			if (Category == ETypeCategory::Reference)
			{
				CE::Type Underlying = Type.Underlying();
				if (m_Type == Underlying)
				{
					return true;
				}

				if (Underlying.IsBaseOf(m_Type))
				{
					return true;
				}
			}
			else if (Category == ETypeCategory::Pointer && m_Type.Category() == ETypeCategory::Pointer)
			{
				CE::Type UnderlyingArg = m_Type.Underlying();
				CE::Type UnderlyingType = Type.Underlying();

				if (UnderlyingArg == UnderlyingType)
				{
					return true;
				}

				if (UnderlyingType.IsBaseOf(UnderlyingArg))
				{
					return true;
				}
			}

			return false;
		}

		// Returns the Casted Value
		template<typename T>
		std::enable_if_t<!std::is_rvalue_reference_v<T>, T> Get() const
		{
			using raw_type = typename std::remove_reference<T>::type;
			return (*reinterpret_cast<raw_type*>(const_cast<void*>(m_Data)));
		}

		// Returns the Casted Value
		template<typename T>
		std::enable_if_t<std::is_rvalue_reference_v<T>, std::remove_reference_t<T>> Get() const
		{
			using raw_type = typename std::remove_reference<T>::type;
			return std::move(*reinterpret_cast<raw_type*>(const_cast<void*>(m_Data)));
		}

	private:
		template<typename T>
		std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>> Emplace(T&& In)
		{
			m_Type = Type::Get(In);
			m_Data = (void*)std::addressof(In);
		}

		template<typename T>
		std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>> Emplace(T&& In)
		{
			m_Type = Type::Get<std::remove_reference_t<T>>();
			m_Data = (void*)In;
		}

	private:
		void* m_Data = nullptr;
		CE::Type m_Type;
	};
}
