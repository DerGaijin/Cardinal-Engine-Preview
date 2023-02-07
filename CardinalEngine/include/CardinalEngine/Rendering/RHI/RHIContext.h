#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIObject.h"
#include "Enums.h"

namespace CE
{
	class RHIImage;
	class RHIBuffer;
	class Sampler;
	class RHIPipeline_Graphic;

	struct ViewportArea
	{
		float X = 0.0f;
		float Y = 0.0f;
		float Width = 1.0f;
		float Height = 1.0f;
		float MinDepth = 0.0f;
		float MaxDepth = 1.0f;
	};

	struct ScissorArea
	{
		int32_t OffsetX = 0;
		int32_t OffsetY = 0;
		int32_t ExtentX = 0;
		int32_t ExtentY = 0;
	};

	class CE_API RHIContext : public RHIObject
	{
	public:
		virtual bool IsDeferred() const
		{
			return true;
		}

	public:
		virtual void BeginRenderpass(const Array<SharedPtr<RHIImage>>& RenderTargets, const SharedPtr<RHIImage>& DepthTarget) = 0;

		virtual void ClearRenderTarget(const SharedPtr<RHIImage>& RenderTarget, const Color& ClearColor) = 0;

		virtual void ClearDepthTarget(const SharedPtr<RHIImage>& DepthTarget, bool ClearDepth, float DepthClearValue, bool ClearStencil, uint8 StencilClearValue) = 0;

		virtual void EndRenderpass() = 0;

		virtual void SetViewports(uint32 ViewportCount, ViewportArea* Viewports) = 0;

		virtual void SetScissors(uint32 ScissorCount, ScissorArea* Scissors) = 0;

		virtual void BindGraphicPipeline(SharedPtr<RHIPipeline_Graphic> GraphicPipeline) = 0;

		virtual void BindVertexBuffers(uint32 VertexBufferCount, SharedPtr<RHIBuffer>* VertexBuffers, uint32* Offsets, uint32 FirstBinding = 0) = 0;

		virtual void BindIndexBuffer(SharedPtr<RHIBuffer> IndexBuffer, EIndexBufferType Type = EIndexBufferType::UInt32, uint32 Offset = 0) = 0;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<RHIBuffer> Buffer) = 0;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<RHIImage> Image) = 0;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<Sampler> Sampler) = 0;

		virtual void DrawPrimitive(uint32 VertexCount, uint32 StartVertex = 0, uint32_t InstanceCount = 1, uint32 FirstInstance = 0) = 0;

		virtual void DrawIndexedPrimitive(uint32 IndexCount, uint32 FirstIndex = 0, uint32 StartVertex = 0, uint32 InstanceCount = 1, uint32 FirstInstance = 0) = 0;

		virtual void CopyImage(SharedPtr<RHIImage> From, SharedPtr<RHIImage> To) = 0;

		virtual void CopyBuffer(SharedPtr<RHIBuffer> From, SharedPtr<RHIBuffer> To) = 0;
	};
}
