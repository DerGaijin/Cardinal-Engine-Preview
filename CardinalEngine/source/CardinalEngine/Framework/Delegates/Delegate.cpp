#include "CardinalEngine/Framework/Delegates/Delegate.h"

namespace CE
{
	void DelegateBase::SetBindingDelegate(DelegateBinding& Binding, DelegateBase* Delegate)
	{
		Binding.m_Delegate = Delegate;
	}

	bool DelegateBinding::IsValid() const
	{
		return m_Delegate != nullptr;
	}

	void DelegateBinding::Unbind()
	{
		if (m_Delegate != nullptr)
		{
			m_Delegate->Unbind(this);
		}
	}
}
