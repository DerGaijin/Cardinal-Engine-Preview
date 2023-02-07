#include "CardinalEngine/Application/Application.h"

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include "Windows/Application_Windows.h"
#endif

namespace CE
{
	Application* Application::Create()
	{
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		return new Application_Windows();
#endif
	}
}
