#pragma once
#include "../Core/Headers.h"
#include "Type.h"
#include "Any.h"

namespace CE
{
	// Contains a Reference to Value to be used as an Handle
	class CE_API Handle
	{
	public:
		// Default Constructor
		Handle() = default;

		// Constructs a Handle from an Any
		Handle(const Any& Any)
		{
			m_Data = Any.m_Data;
			m_Type = Any.m_Type;
			if (m_Type.Category() == ETypeCategory::Reference)
			{
				m_Type = m_Type.Underlying();
			}
		}

		// Constructs a Handle from any Object and copies it
		template<typename T, typename = typename std::enable_if_t<!std::is_same_v<T, Any>>>
		Handle(const T& Inst)
		{
			m_Data = std::addressof(const_cast<T&>(Inst));
			m_Type = Type::Get<T>();
		}
	
		// Constructs a Handle from any Object and moves it
		template<typename T, typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, Any>>>
		Handle(T&& Inst)
		{
			m_Data = std::addressof(Inst);
			m_Type = Type::Get<std::remove_reference_t<T>>();
		}

		// Constructs a Handle from a Pointer, can be Resolved to the Instance
		template<typename T>
		Handle(T* Inst, bool Resolve = true)
		{
			if (Resolve)
			{
				using RawT = RawType_t<T>;

				RawT* Ptr = ResolvePointer(Inst);
				m_Data = Ptr;
				m_Type = Type::Get<RawT>();
			}
			else
			{
				m_Data = Inst;
				m_Type = Type::Get<T>();
			}
		}

		// Returns true if the Handle has a valid Value
		bool IsValid() const
		{
			return m_Data != nullptr && m_Type.IsValid();
		}

		// Returns the Type of the Handle
		const Type& Type() const
		{
			return m_Type;
		}

		// Returns the Casted Value Pointer
		template<typename T>
		T* Get() const
		{
			if (m_Data != nullptr && m_Type.IsValid())
			{
				CE::Type TType = CE::Type::Get<T>();
				if (TType.IsValid() && (TType == m_Type || TType.IsBaseOf(m_Type)))
				{
					return reinterpret_cast<T*>(m_Data);
				}
			}
			return nullptr;
		}

	private:
		template<typename T>
		std::enable_if_t<std::is_pointer_v<T>, RawType_t<T>*> ResolvePointer(T* Ptr)
		{
			return ResolvePointer(*Ptr);
		}

		template<typename T>
		std::enable_if_t<!std::is_pointer_v<T>, T*> ResolvePointer(T* Ptr)
		{
			return Ptr;
		}

	private:
		void* m_Data = nullptr;
		CE::Type m_Type;
	};
}
