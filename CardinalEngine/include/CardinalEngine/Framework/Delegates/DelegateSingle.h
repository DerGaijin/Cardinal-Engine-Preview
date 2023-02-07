#pragma once
#include "../Core/Headers.h"
#include "Delegate.h"

namespace CE
{
	template<typename Fn>
	class DelegateSingle;

	template<typename Ret, typename... ARGS>
	class DelegateSingle<Ret(ARGS...)> : public Delegate<Ret(ARGS...)>
	{
	public:
		DelegateSingle() = default;

		DelegateSingle(DelegateSingle&& Other) noexcept
		{
			*this = std::move(Other);
		}
		
		DelegateSingle& operator=(DelegateSingle&& Other) noexcept
		{
			std::swap(m_Callback, Other.m_Callback);
			std::swap(m_Binding, Other.m_Binding);
			if (m_Binding != nullptr)
			{
				SetBindingDelegate(*m_Binding, this);
			}
			if (Other.m_Binding != nullptr)
			{
				SetBindingDelegate(*Other.m_Binding, &Other);
			}
			return *this;
		}

		~DelegateSingle()
		{
			Clear();
		}

		virtual UniquePtr<DelegateBinding> Bind(const FunctionRef<Ret(ARGS...)>& Callback) override
		{
			Clear();
			m_Callback = Callback;
			UniquePtr<DelegateBinding> Binding = new DelegateBinding();
			m_Binding = Binding.Get();
			SetBindingDelegate(*Binding, this);
			return Binding;
		}

		virtual bool Unbind(DelegateBinding* Binding) override
		{
			if (m_Binding != nullptr && Binding == m_Binding)
			{
				Clear();
				return true;
			}
			return false;
		}

		virtual void Clear() override
		{
			m_Callback = nullptr;
			if (m_Binding != nullptr)
			{
				SetBindingDelegate(*m_Binding, nullptr);
			}
			m_Binding = nullptr;
		}

		virtual bool IsBound() const override
		{
			return m_Binding != nullptr && m_Callback != nullptr;
		}

		virtual Ret Execute(ARGS... Args) override
		{
			CE_ASSERT(IsBound(), "Tried to execute DelegateSingle with no callback bound");
			return m_Callback(std::forward<ARGS>(Args)...);
		}

	private:
		FunctionRef<Ret(ARGS...)> m_Callback;
		DelegateBinding* m_Binding = nullptr;
	};
}
