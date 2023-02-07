#pragma once
#include "../../Framework/Core/Headers.h"
#include "TransformComponent.h"

namespace CE
{
	class RP_Light;

	class CE_API LightComponent : public TransformComponent
	{
		CE_OBJECT_BODY();
	public:

	protected:
		virtual SharedPtr<RenderProxy> GetRenderProxy() const override;

	protected:
		SharedPtr<RP_Light> m_RenderProxy;
	};
}
