#include "CardinalEngine/Framework/Core/Definitions.h"

#if CE_STANDALONE
#error CardinalEngine-Editor cannot be build in Standalone Configurations
#endif

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include "Entrypoint_Windows.h"
#else
#error No Entrypoint for this Platform
#endif
