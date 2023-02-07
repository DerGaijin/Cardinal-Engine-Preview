#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class CE_API AudioThread : public NonCopyable
	{
		friend class TaskSystem;
	public:


	private:
		void AudioThreadMain();

	private:
		std::thread m_Thread;
	};
}
