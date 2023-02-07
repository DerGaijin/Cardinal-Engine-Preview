#pragma once
#include "../Math/Fundamentals.h"

#include <memory>
#include <atomic>

namespace CE
{
	class RefCountBlock
	{
	public:
		bool IncRefNZ()
		{
			uint64 Expected = m_Uses.load();
			while (Expected != 0)
			{
				if (m_Uses.compare_exchange_strong(Expected, Expected + 1))
				{
					IncWRef();
					return true;
				}
			}
			return false;
		}

		void IncRef()
		{
			m_Uses++;
			IncWRef();
		}

		void IncWRef()
		{
			m_Weaks++;
		}

		void DecRef()
		{
			if (--m_Uses == 0)
			{
				Destroy();
			}
			DecWRef();
		}

		void DecWRef()
		{
			if (--m_Weaks == 0)
			{
				Delete();
			}
		}

		uint64 UseCount() const
		{
			return m_Uses.load();
		}

		uint64 WeakCount() const
		{
			return m_Weaks.load();
		}

	protected:
		virtual void Destroy() = 0;

		virtual void Delete() = 0;

	public:
		std::atomic<uint64> m_Uses = 1;
		std::atomic<uint64> m_Weaks = 1;
	};

	template<typename T>
	class RefCountBlock_NoDeleter : public RefCountBlock
	{
	public:
		RefCountBlock_NoDeleter(T* Ptr) : m_Ptr(Ptr) {}

	protected:
		virtual void Destroy() override
		{
			delete(m_Ptr);
		}

		virtual void Delete() override
		{
			delete(this);
		}

	private:
		T* m_Ptr = nullptr;
	};

	template<typename T, typename Deleter>
	class RefCountBlock_Deleter : public RefCountBlock
	{
	public:
		RefCountBlock_Deleter(T* Ptr, const Deleter& Copy) : m_Ptr(Ptr), m_Deleter(Copy) {}

		RefCountBlock_Deleter(T* Ptr, Deleter&& Move) : m_Ptr(Ptr), m_Deleter(Move) {}

	protected:
		virtual void Destroy() override
		{
			m_Deleter(m_Ptr);
		}

		virtual void Delete() override
		{
			delete(this);
		}

	private:
		T* m_Ptr = nullptr;
		Deleter m_Deleter;
	};

	template<typename T>
	class WeakPtr;

	template<typename T>
	class SharedPtr
	{
	public:
		using ElementT = std::remove_cv_t<T>;

	private:
		friend class WeakPtr<T>;

		template<typename P>
		SharedPtr(P* Data, RefCountBlock* RefCount, bool EnableShared = false) : m_Data(const_cast<ElementT*>(Data)), m_RefCount(RefCount)
		{
			if (EnableShared)
			{
				EnableSharedFromThis(RefCount);
			}
		}

	public:
		SharedPtr() = default;

		SharedPtr(std::nullptr_t)
		{

		}

		explicit SharedPtr(T* Ptr) : m_Data(const_cast<ElementT*>(Ptr)), m_RefCount(new RefCountBlock_NoDeleter<ElementT>(m_Data))
		{
			EnableSharedFromThis(m_RefCount);
		}

		template<typename Deleter = std::default_delete<ElementT>>
		static SharedPtr Create(T* Ptr)
		{
			return SharedPtr<T>(Ptr, new RefCountBlock_Deleter<ElementT, Deleter>(Ptr, Deleter()), true);
		}

		template<typename Deleter = std::default_delete<ElementT>>
		static SharedPtr Create(T* Ptr, const Deleter& Delete)
		{
			return SharedPtr<T>(Ptr, new RefCountBlock_Deleter<ElementT, Deleter>(Ptr, Delete), true);
		}

		template<typename Deleter = std::default_delete<ElementT>>
		static SharedPtr Create(T* Ptr, Deleter&& Delete)
		{
			return SharedPtr<T>(Ptr, new RefCountBlock_Deleter<ElementT, Deleter>(Ptr, Delete), true);
		}

		template<typename I>
		static SharedPtr<T> StaticCast(const SharedPtr<I>& ToCast)
		{
			T* Casted = static_cast<T*>(ToCast.Get());
			ToCast.m_RefCount->IncRef();
			return SharedPtr<T>(Casted, ToCast.m_RefCount);
		}

		template<typename I>
		static SharedPtr<T> DynamicCast(const SharedPtr<I>& ToCast)
		{
			T* Casted = dynamic_cast<T*>(ToCast.Get());
			if (Casted != nullptr)
			{
				ToCast.m_RefCount->IncRef();
			}
			return SharedPtr<T>(Casted, Casted != nullptr ? ToCast.m_RefCount : nullptr);
		}

		template<typename I>
		static SharedPtr<T> ConstCast(const SharedPtr<I>& ToCast)
		{
			T* Casted = const_cast<T*>(ToCast.Get());
			ToCast.m_RefCount->IncRef();
			return SharedPtr<T>(Casted, ToCast.m_RefCount);
		}

		SharedPtr(const SharedPtr& Copy)
		{
			m_Data = Copy.m_Data;
			m_RefCount = Copy.m_RefCount;
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncRef();
			}
		}

		template<typename O, std::enable_if_t<std::is_convertible_v<O*, T*>, int> = 0>
		SharedPtr(const SharedPtr<O>& Copy)
		{
			m_Data = Copy.m_Data;
			m_RefCount = Copy.m_RefCount;
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncRef();
			}
		}

		SharedPtr(SharedPtr&& Move) noexcept
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_RefCount, Move.m_RefCount);
		}

		SharedPtr& operator=(const SharedPtr& Copy)
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecRef();
			}
			m_Data = Copy.m_Data;
			m_RefCount = Copy.m_RefCount;
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncRef();
			}
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& Move) noexcept
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_RefCount, Move.m_RefCount);
			return *this;
		}

		~SharedPtr()
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecRef();
			}
		}

		bool IsValid() const
		{
			return m_Data != nullptr && m_RefCount != nullptr;
		}

		bool IsUnique() const
		{
			if (m_RefCount != nullptr)
			{
				return m_RefCount->UseCount() == 1;
			}
			return false;
		}

		uint64 UseCount() const
		{
			if (m_RefCount != nullptr)
			{
				return m_RefCount->UseCount();
			}
			return 0;
		}

		T* Get() const
		{
			return m_Data;
		}

		T* Get()
		{
			return m_Data;
		}

		void Reset()
		{
			m_Data = nullptr;
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecRef();
			}
			m_RefCount = nullptr;
		}

	public:
		operator bool() const
		{
			return IsValid();
		}

		T* operator->() const
		{
			return Get();
		}

		T* operator->()
		{
			return Get();
		}

		T& operator*() const
		{
			return *Get();
		}

		T& operator*()
		{
			return *Get();
		}

		template<typename O>
		bool operator==(const SharedPtr<O>& Other)const
		{
			return m_Data == Other.m_Data;
		}

		bool operator==(const std::nullptr_t&)const
		{
			return m_Data == nullptr;
		}

		template<typename O>
		bool operator!=(const SharedPtr<O>& Other)const
		{
			return !(*this == Other);
		}

		bool operator!=(const std::nullptr_t&)const
		{
			return m_Data != nullptr;
		}

		template<typename O>
		bool operator<(const SharedPtr<O>& Other)const
		{
			return m_Data < Other.m_Data;
		}

	private:
		void EnableSharedFromThis(RefCountBlock* RefCount);

	public:
		ElementT* m_Data = nullptr;
		RefCountBlock* m_RefCount = nullptr;
	};

	template<typename T>
	class WeakPtr
	{
	public:
		using ElementT = std::remove_cv_t<T>;

	public:
		WeakPtr() = default;

		WeakPtr(const SharedPtr<T>& Shared) : m_Data(Shared.m_Data), m_RefCount(Shared.m_RefCount)
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncWRef();
			}
		}

		template<typename O, std::enable_if_t<std::is_convertible_v<O*, T*>, int> = 0>
		WeakPtr(const SharedPtr<O>& Copy) : m_Data(Copy.m_Data), m_RefCount(Copy.m_RefCount)
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncWRef();
			}
		}

		WeakPtr(const WeakPtr& Copy)
		{
			m_Data = Copy.m_Data;
			m_RefCount = Copy.m_RefCount;
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncWRef();
			}
		}

		WeakPtr(WeakPtr&& Move) noexcept
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_RefCount, Move.m_RefCount);
		}

		WeakPtr& operator=(const WeakPtr& Copy)
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecWRef();
			}
			m_Data = Copy.m_Data;
			m_RefCount = Copy.m_RefCount;
			if (m_RefCount != nullptr)
			{
				m_RefCount->IncWRef();
			}
			return *this;
		}

		WeakPtr& operator=(WeakPtr&& Move) noexcept
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_RefCount, Move.m_RefCount);
			return *this;
		}

		~WeakPtr()
		{
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecWRef();
			}
		}

		bool Expired() const
		{
			if (m_RefCount != nullptr)
			{
				return m_RefCount->UseCount() == 0;
			}
			return true;
		}

		SharedPtr<T> Lock() const
		{
			if (m_RefCount != nullptr && m_RefCount->IncRefNZ())
			{
				return SharedPtr<T>(m_Data, m_RefCount);
			}
			return nullptr;
		}

		uint64 WeakCount() const
		{
			if (m_RefCount != nullptr)
			{
				return m_RefCount->WeakCount();
			}
			return 0;
		}

		void Reset()
		{
			m_Data = nullptr;
			if (m_RefCount != nullptr)
			{
				m_RefCount->DecWRef();
			}
			m_RefCount = nullptr;
		}

	private:
		ElementT* m_Data = nullptr;
		RefCountBlock* m_RefCount = nullptr;
	};

	template<typename T>
	class SharedFromThis
	{
		template<typename O>
		friend class SharedPtr;

	public:
		using WeakType = T;

		SharedPtr<T> GetShared() const
		{
			return m_Weak.Lock();
		}

		WeakPtr<T> GetWeak() const noexcept
		{
			return m_Weak;
		}

	private:
		WeakPtr<T> m_Weak;
	};

	template<typename T, typename = void>
	struct CanEnabledShared : std::false_type {};

	template<typename T>
	struct CanEnabledShared<T, std::void_t<typename T::WeakType>> : std::is_convertible<std::remove_cv_t<T>*, typename T::WeakType*>::type {};

	template<typename T>
	void SharedPtr<T>::EnableSharedFromThis(RefCountBlock* RefCount)
	{
		if constexpr (CanEnabledShared<T>::value)
		{
			if (m_Data != nullptr)
			{
				RefCount->IncRef();
				m_Data->m_Weak = SharedPtr<T>(m_Data, RefCount);
			}
		}
	}

	template<typename T, typename Deleter = std::default_delete<T>>
	class UniquePtr
	{
		template<typename O, typename Del>
		friend class UniquePtr;

	public:
		UniquePtr() = default;

		UniquePtr(T* Ptr) : m_Data(Ptr)
		{

		}

		UniquePtr(const UniquePtr& Copy) = delete;

		UniquePtr(UniquePtr&& Move)
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_Deleter, Move.m_Deleter);
		}

		template<typename O, typename Del = Deleter, std::enable_if_t<std::is_convertible_v<O*, T*>, int> = 0>
		UniquePtr(UniquePtr<O, Del>&& Move)
		{
			m_Data = Move.m_Data;
			Move.Release();
		}

		UniquePtr& operator=(const UniquePtr& Copy) = delete;

		UniquePtr& operator=(UniquePtr&& Move)
		{
			std::swap(m_Data, Move.m_Data);
			std::swap(m_Deleter, Move.m_Deleter);
			return *this;
		}

		~UniquePtr()
		{
			if (m_Data != nullptr)
			{
				m_Deleter(m_Data);
				m_Data = nullptr;
			}
		}

		T* Get() const
		{
			return m_Data;
		}

		T* Get()
		{
			return m_Data;
		}

		void Reset()
		{
			if (m_Data)
			{
				m_Deleter(m_Data);
				m_Data = nullptr;
			}
		}

		void Release()
		{
			m_Data = nullptr;
		}

	public:
		operator bool() const
		{
			return m_Data != nullptr;
		}

		T* operator->() const
		{
			return Get();
		}

		T* operator->()
		{
			return Get();
		}

		T& operator*() const
		{
			return *Get();
		}

		T& operator*()
		{
			return *Get();
		}

		bool operator==(const UniquePtr& Other) const
		{
			return m_Data == Other;
		}

		bool operator==(const std::nullptr_t&) const
		{
			return m_Data == nullptr;
		}

		bool operator!=(const UniquePtr& Other) const
		{
			return !(*this == Other);
		}

		bool operator!=(const std::nullptr_t&) const
		{
			return m_Data != nullptr;
		}

		bool operator<(const UniquePtr& Other) const
		{
			return m_Data < Other.m_Data;
		}

	private:
		T* m_Data = nullptr;
		Deleter m_Deleter;
	};
}
