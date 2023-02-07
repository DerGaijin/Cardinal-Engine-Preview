#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class CE_API Monitor
	{
	public:
		Monitor(void* NativeHandle, Vec2f MainPos, Vec2f MainSize, Vec2f WorkPos, Vec2f WorkSize)
			: m_NativeHandle(NativeHandle), m_MainPos(MainPos), m_MainSize(MainPos), m_WorkPos(WorkPos), m_WorkSize(WorkSize) {}

		void* GetNativeHandle() const
		{
			return m_NativeHandle;
		}

		const Vec2f& MainPos() const
		{
			return m_MainPos;
		}

		const Vec2f& MainSize() const
		{
			return m_MainSize;
		}

		const Vec2f& WorkPos() const
		{
			return m_WorkSize;
		}

		const Vec2f& WorkSize() const
		{
			return m_WorkSize;
		}

	private:
		void* m_NativeHandle = nullptr;

		Vec2f m_MainPos = Vec2f(0.0f);
		Vec2f m_MainSize = Vec2f(0.0f);
		Vec2f m_WorkPos = Vec2f(0.0f);
		Vec2f m_WorkSize = Vec2f(0.0f);
	};
}
