#pragma once
#include "../Definitions.h"
#include "Singleton.h"
#include "../Containers/FunctionRef.h"

namespace CE
{
	class CE_API ScopeGuard : public NonCopyable
	{
	public:
		// Constructor with Callback
		ScopeGuard(const FunctionRef<void()>& Callback) : m_Callback(Callback) {}

		// Move Constructor
		ScopeGuard(ScopeGuard&& Other) noexcept : m_Callback(Other.m_Callback)
		{
			Other.Dismiss();
		}

		// Destructor
		~ScopeGuard()
		{
			if (m_Callback)
			{
				m_Callback();
			}
		}

		// Invalidates the ScopeGuard
		void Dismiss()
		{
			m_Callback = nullptr;
		}

		void ExecuteAndDismiss()
		{
			if (m_Callback)
			{
				m_Callback();
			}
			m_Callback = nullptr;
		}

	private:
		FunctionRef<void()> m_Callback;
	};
}
