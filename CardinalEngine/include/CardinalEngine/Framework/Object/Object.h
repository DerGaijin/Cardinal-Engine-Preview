#pragma once
#include "../Core/Headers.h"
#include "../Reflection/Type.h"

#define CE_OBJECT_BODY() \
	CE_REFLECTION_BODY()

namespace CE
{
	// The Base Object of the Framework
	class CE_API Object : public SharedFromThis<Object>
	{
		CE_OBJECT_BODY()
	public:
		virtual ~Object() = default;



	public:
#if CE_EDITOR
		String EditorName;
#endif
	};
}
