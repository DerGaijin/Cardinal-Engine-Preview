#pragma once
#include "../Core/Headers.h"
#include "Type.h"

namespace CE
{
	// Contains any type (eg. Classes, Pointers, References)
	class CE_API Any
	{
		friend class Handle;
		friend class Argument;
	public:
		typedef void(*CopyFn)(void*&, const void*);
		typedef void(*MoveFn)(void*&, void*);
		typedef void(*DeleteFn)(void*);

	private:
		template<typename T>
		static std::enable_if_t<std::is_copy_constructible_v<T>> CopyInstance(void*& To, const void* From)
		{
			To = new T(*reinterpret_cast<const T*>(From));
		}

		template<typename T>
		static std::enable_if_t<!std::is_copy_constructible_v<T>> CopyInstance(void*& To, const void* From)
		{

		}

		template<typename T>
		static std::enable_if_t<std::is_move_constructible_v<T>> MoveInstance(void*& To, void* From)
		{
			To = new T(std::move(*reinterpret_cast<T*>(From)));
		}

		template<typename T>
		static std::enable_if_t<!std::is_move_constructible_v<T>> MoveInstance(void*& To, void* From)
		{

		}

		template<typename T>
		static void DeleteInstance(void* Instance)
		{
			delete(reinterpret_cast<T*>(Instance));
		}

		static void CopyPointer(void*& To, const void* From)
		{
			To = const_cast<void*>(From);
		}

		static void MovePointer(void*& To, void* From)
		{
			To = From;
		}

	public:
		// Create a Any type with a Copy of Value
		template<typename T>
		static std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>, Any> Create(const T& Value)
		{
			CopyFn Copy = nullptr;
			if (std::is_copy_constructible_v<T>)
			{
				Copy = &CopyInstance<T>;
			}
			MoveFn Move = nullptr;
			if (std::is_move_constructible_v<T>)
			{
				Move = &MoveInstance<T>;
			}
			DeleteFn Delete = &DeleteInstance<T>;

			T* NewValue = new T(Value);
			return Any(NewValue, Type::Get(Value), Copy, Move, Delete);
		}

		// Create a Any type move with Value
		template<typename T>
		static std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>&& std::is_move_constructible_v<T>, Any> Create(T&& Value)
		{
			CopyFn Copy = nullptr;
			if (std::is_copy_constructible_v<T>)
			{
				Copy = &CopyInstance<T>;
			}
			MoveFn Move = nullptr;
			if (std::is_move_constructible_v<T>)
			{
				Move = &MoveInstance<T>;
			}
			DeleteFn Delete = &DeleteInstance<T>;

			T* NewValue = new T(std::move(Value));
			return Any(NewValue, Type::Get(Value), Copy, Move, Delete);
		}

		// Create a Any type as a Reference
		template<typename T>
		static std::enable_if_t<std::is_reference_v<T>, Any> Create(const T& Value)
		{
			CopyFn Copy = &CopyPointer;
			MoveFn Move = &MovePointer;
			DeleteFn Delete = nullptr;
			return Any((void*)&Value, Type::Get<T>(), Copy, Move, Delete);
		}

		// Create a Any type as a Pointer
		template<typename T>
		static std::enable_if_t<std::is_pointer_v<T>, Any> Create(T Value)
		{
			CopyFn Copy = &CopyPointer;
			MoveFn Move = &MovePointer;
			DeleteFn Delete = nullptr;
			return Any((void*)Value, Type::Get<T>(), Copy, Move, Delete);
		}

	public:
		// Default Any
		Any() = default;

		// Any Constructor
		Any(void* Data, const Type& InType, CopyFn InCopy, MoveFn InMove, DeleteFn InDelete) : m_Data(Data), m_Type(InType), m_Copy(InCopy), m_Move(InMove), m_Delete(InDelete) {}

		// Copy Constructor
		Any(const Any& Other)
		{
			*this = std::forward<const Any&>(Other);
		}

		// Copy Operator
		Any& operator=(const Any& Other);

		// Move Constructor
		Any(Any&& Other) noexcept
		{
			*this = std::forward<Any&&>(Other);
		}

		// Move Operator
		Any& operator=(Any&& Other) noexcept;

		// Default Destructor
		~Any()
		{
			if (m_Data != nullptr && m_Delete != nullptr)
			{
				m_Delete(m_Data);
			}
		}

	public:
		// Returns true if the Any contains a valid Value
		bool IsValid() const
		{
			return m_Data != nullptr && m_Type.IsValid();
		}

		// Returns the Type of contained Value
		Type Type() const
		{
			return m_Type;
		}

		// Returns true if the Any can be Copied
		bool IsCopyable() const
		{
			return m_Copy != nullptr;
		}

		// Returns true if the Any can be Moved
		bool IsMoveable() const
		{
			return m_Move != nullptr;
		}

		// Clears the Any
		void Clear();


		// Returns a Pointer to the Value
		template<typename T>
		std::enable_if_t<!std::is_pointer_v<T> && !std::is_reference_v<T>, T*> GetValue() const
		{
			CE::Type TType = Type::Get<T>();
			if (TType == m_Type || TType.IsBaseOf(m_Type))
			{
				return reinterpret_cast<T*>(m_Data);
			}
			return nullptr;
		}

		// Returns a Pointer to the Value
		template<typename T>
		std::enable_if_t<std::is_reference_v<T>, std::remove_reference_t<T>*> GetValue() const
		{
			CE::Type TType = Type::Get<T>();
			if (TType == m_Type)
			{
				return reinterpret_cast<std::remove_reference_t<T>*>(m_Data);
			}

			CE::Type UTType = TType.Underlying();
			if (TType.Category() == ETypeCategory::Reference && (UTType == m_Type || UTType.IsBaseOf(m_Type)))
			{
				return reinterpret_cast<std::remove_reference_t<T>*>(m_Data);
			}
			return nullptr;
		}

		// Returns a Pointer to the Value
		template<typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> GetValue() const
		{
			CE::Type TType = Type::Get<T>();
			if (TType == m_Type)
			{
				return reinterpret_cast<T>(m_Data);
			}

			CE::Type UTType = TType.Underlying();
			CE::Type UType = m_Type.Underlying();
			if (m_Type.Category() == ETypeCategory::Pointer && (UType == UTType || UTType.IsBaseOf(UType)))
			{
				return reinterpret_cast<T>(m_Data);
			}

			return nullptr;
		}


		// Returns a Pointer to the Value and Clears the Any
		template<typename T>
		std::enable_if_t<!std::is_pointer_v<T> && !std::is_reference_v<T>, T*> ExtractValue()
		{
			T* Result = GetValue<T>();
			if (Result != nullptr)
			{
				m_Data = nullptr;
				Clear();
			}
			return Result;
		}

		// Returns a Pointer to the Value and Clears the Any
		template<typename T>
		std::enable_if_t<std::is_reference_v<T>, std::remove_reference_t<T>*> ExtractValue()
		{
			std::remove_reference_t<T>* Result = GetValue<T>();
			if (Result != nullptr)
			{
				m_Data = nullptr;
				Clear();
			}
			return Result;
		}

		// Returns a Pointer to the Value and Clears the Any
		template<typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> ExtractValue()
		{
			T Result = GetValue<T>();
			if (Result != nullptr)
			{
				m_Data = nullptr;
				Clear();
			}
			return Result;
		}

	private:
		void* m_Data = nullptr;
		CE::Type m_Type;

		CopyFn m_Copy = nullptr;
		MoveFn m_Move = nullptr;
		DeleteFn m_Delete = nullptr;
	};
}
