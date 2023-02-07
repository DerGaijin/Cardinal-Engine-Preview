#pragma once
#include "../../Framework/Core/Headers.h"
#include "RenderProxy.h"

namespace CE
{
	class RenderTarget;
	class RHIBuffer;
	class RHIImage;
	class RHIContextImmediate;

	class CE_API RP_Camera : public RenderProxy
	{
	public:
		struct ProjViewBuffData
		{
			Mat4x4f Projection;
			Mat4x4f View;
			Mat4x4f ProjectionView;
			alignas(16) Vec3f ViewPos;
		};

	public:
		RP_Camera();

		void SetRenderTarget(const SharedPtr<RenderTarget>& RenderTarget);

		SharedPtr<RenderTarget> GetRenderTarget() const
		{
			return m_RenderTarget;
		}

		SharedPtr<RHIImage> GetDepthTarget() const
		{
			return m_DepthTarget;
		}

		void SetProjectionData(RHIContextImmediate& Context, const ProjViewBuffData& NewData);

		SharedPtr<RHIBuffer> GetProjectionViewBuffer() const
		{
			return m_ProjectionViewBuffer;
		}

	private:
		SharedPtr<RenderTarget> m_RenderTarget;
		SharedPtr<RHIImage> m_DepthTarget;

		SharedPtr<RHIBuffer> m_ProjectionViewBuffer;
	};
}
