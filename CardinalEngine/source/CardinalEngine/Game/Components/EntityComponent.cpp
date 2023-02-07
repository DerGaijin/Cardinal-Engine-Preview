#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Entity.h"

namespace CE
{
	SharedPtr<Entity> EntityComponent::Entity() const
	{
		return m_Entity != nullptr ? SharedPtr<CE::Entity>::DynamicCast(m_Entity->GetShared()) : nullptr;
	}
}
