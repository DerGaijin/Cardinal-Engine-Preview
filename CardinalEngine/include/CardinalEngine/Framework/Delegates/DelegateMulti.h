#pragma once
#include "../Core/Headers.h"
#include "Delegate.h"

namespace CE
{
	template<typename... ARGS>
	class DelegateMulti : public Delegate<void(ARGS...)>
	{
	public:
		DelegateMulti() = default;

		DelegateMulti(DelegateMulti&& Other) noexcept
		{
			*this = std::move(Other);
		}

		DelegateMulti& operator=(DelegateMulti&& Other) noexcept
		{
			std::swap(m_Callbacks, Other.m_Callbacks);
			for (auto& It : m_Callbacks)
			{
				SetBindingDelegate(*It.first, this);
			}
			for (auto& It : Other.m_Callbacks)
			{
				SetBindingDelegate(*It.first, &Other);
			}
			return *this;
		}

		~DelegateMulti()
		{
			Clear();
		}

		virtual UniquePtr<DelegateBinding> Bind(const FunctionRef<void(ARGS...)>& Callback) override
		{
			UniquePtr<DelegateBinding> Binding = new DelegateBinding();
			SetBindingDelegate(*Binding, this);
			m_Callbacks[Binding.Get()] = Callback;
			return Binding;
		}

		virtual bool Unbind(DelegateBinding* Binding) override
		{
			auto Found = m_Callbacks.find(Binding);
			if (Found != m_Callbacks.end())
			{
				SetBindingDelegate(*Binding, nullptr);
				m_Callbacks.erase(Found);
				return true;
			}
			return false;
		}

		virtual void Clear() override
		{
			for (auto& It : m_Callbacks)
			{
				SetBindingDelegate(*It.first, nullptr);
			}
			m_Callbacks.clear();
		}

		virtual bool IsBound() const override
		{
			return m_Callbacks.size() > 0;
		}

		virtual void Execute(ARGS... Args) override
		{
			for (auto& It : m_Callbacks)
			{
				It.second(Args...);
			}
		}

	private:
		Map<DelegateBinding*, FunctionRef<void(ARGS...)>> m_Callbacks;
	};
}
