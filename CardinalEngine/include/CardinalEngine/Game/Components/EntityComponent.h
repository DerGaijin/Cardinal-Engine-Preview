#pragma once
#include "../../Framework/Core/Headers.h"
#include "../../Framework/Object/Object.h"

namespace CE
{
	class Entity;

	class CE_API EntityComponent : public Object
	{
		CE_OBJECT_BODY();
	private:
		friend class Entity;

	public:

	public:
		virtual SharedPtr<Entity> Entity() const;

#if CE_EDITOR
	protected:
		friend class MainEditor;
		virtual void DrawInspector() {}
#endif

	private:
		CE::Entity* m_Entity = nullptr;
	};
}
