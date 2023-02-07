#include "CardinalEngine/Game/Components/LightComponent.h"
#include "CardinalEngine/Game/RenderProxies/RP_Light.h"

namespace CE
{
	SharedPtr<RenderProxy> LightComponent::GetRenderProxy() const
	{
		return m_RenderProxy;
	}
}
