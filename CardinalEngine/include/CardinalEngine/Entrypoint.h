#pragma once
#include "Framework/Core/Definitions.h"

#if CE_STANDALONE

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include "Entrypoints/Entrypoint_Windows.h"
#else
#error No Entrypoint for this Platform
#endif

#endif
