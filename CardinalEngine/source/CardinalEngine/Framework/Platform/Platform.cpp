#include "CardinalEngine/Framework/Platform/Platform.h"

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include "Windows/Platform_Windows.h"
#endif


namespace CE
{
	Platform& Platform::Get()
	{
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		static Platform_Windows Inst;
		return Inst;
#else
#error Platform implementation missing for this Platform
#endif
	}
}
