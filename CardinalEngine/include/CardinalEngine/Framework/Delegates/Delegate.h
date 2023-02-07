#pragma once
#include "../Core/Headers.h"

namespace CE
{
	template<typename Fn>
	class Delegate;

	class DelegateBinding;
	class CE_API DelegateBase : public NonCopyable
	{
	public:
		virtual bool Unbind(DelegateBinding* Binding) = 0;

		virtual void Clear() = 0;

		virtual bool IsBound() const = 0;

	protected:
		static void SetBindingDelegate(DelegateBinding& Binding, DelegateBase* Delegate);
	};

	class CE_API DelegateBinding : public Singleton
	{
		friend class DelegateBase;
	public:
		bool IsValid() const;

		void Unbind();

	private:
		DelegateBase* m_Delegate = nullptr;
	};

	template<typename Ret, typename... ARGS>
	class Delegate<Ret(ARGS...)> : public DelegateBase
	{
	public:
		virtual UniquePtr<DelegateBinding> Bind(const FunctionRef<Ret(ARGS...)>& Callback) = 0;

		virtual Ret Execute(ARGS... Args) = 0;
	};
}
