#pragma once
#include "../../Framework/Core/Headers.h"
#include "TransformComponent.h"
#include "../RenderProxies/RP_Camera.h"

namespace CE
{
	class RenderTarget;

	enum class ECameraMode
	{
		Perspective,
		Orthographic,
	};

	class CE_API CameraComponent : public TransformComponent
	{
		CE_OBJECT_BODY();
	public:
		CameraComponent();
	
		void SetRenderTarget(const SharedPtr<RenderTarget>& RenderTarget);

		SharedPtr<RenderTarget> GetRenderTarget() const
		{
			return m_RenderTarget;
		}

	private:
		void CalculateProjection(RP_Camera::ProjViewBuffData& Data);

	protected:
		virtual void UpdateWorldTransform() override;

		virtual SharedPtr<RenderProxy> GetRenderProxy() const override;

	public:
		ECameraMode CameraMode = ECameraMode::Perspective;
		float FOV = 45.0f;
		float ZNear = 0.1f;
		float ZFar = 1000.0f;

	private:
		SharedPtr<RP_Camera> m_RenderProxy;

		SharedPtr<RenderTarget> m_RenderTarget;

#if CE_EDITOR
	protected:
		virtual void DrawInspector() override;
#endif
	};
}
