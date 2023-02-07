#include "CardinalEngine/Game/RenderProxies/RP_Camera.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Rendering/RenderTarget.h"

namespace CE
{
	RP_Camera::RP_Camera()
	{
		RHIBufferDescription Desc;
		Desc.Size = sizeof(ProjViewBuffData);
		Desc.ElementSize = sizeof(ProjViewBuffData);
		Desc.Usage = ERHIBufferUsage::UniformBuffer;
		Desc.Access = ERHIAccess::GPURead_CPUWrite;	// Maybe Use Staging
		m_ProjectionViewBuffer = Engine::Get().DynamicRHI().CreateBuffer(Desc);
	}

	void RP_Camera::SetRenderTarget(const SharedPtr<RenderTarget>& RenderTarget)
	{
		m_RenderTarget = RenderTarget;
		SharedPtr<RHIImage> RTImage = m_RenderTarget->GetTargetImage();
		if (RTImage != nullptr && (m_DepthTarget == nullptr || (RTImage->Description.Width != m_DepthTarget->Description.Width || RTImage->Description.Height != m_DepthTarget->Description.Height)))
		{
			RHIImageDescription Desc;
			Desc.Usage = ERHIImageUsage::DepthStencil;
			Desc.Width = RTImage->Description.Width;
			Desc.Height = RTImage->Description.Height;
			Desc.Format = EDataFormat::D32_Float;
			Desc.Access = ERHIAccess::GPUOnly;
			m_DepthTarget = Engine::Get().DynamicRHI().CreateImage(Desc);
		}
	}

	void RP_Camera::SetProjectionData(RHIContextImmediate& Context, const ProjViewBuffData& NewData)
	{
		void* Mapped = Context.Map(m_ProjectionViewBuffer, ERHIMapAccess::Write);
		if (Mapped == nullptr)
		{
			return;
		}
		memcpy(Mapped, &NewData, sizeof(ProjViewBuffData));
		Context.UnMap(m_ProjectionViewBuffer);
	}
}
