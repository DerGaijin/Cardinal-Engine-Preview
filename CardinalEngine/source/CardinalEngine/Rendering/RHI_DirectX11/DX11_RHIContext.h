#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/RHIContext.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class CE_API DX11_RHIContext : public RHIContext
	{
	public:
		DX11_RHIContext(DX11_DynamicRHI* Owner, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Context);

	public:
		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	public:
		virtual void BeginRenderpass(const Array<SharedPtr<RHIImage>>& RenderTargets, const SharedPtr<RHIImage>& DepthTarget) override;

		virtual void ClearRenderTarget(const SharedPtr<RHIImage>& RenderTarget, const Color& ClearColor) override;

		virtual void ClearDepthTarget(const SharedPtr<RHIImage>& DepthTarget, bool ClearDepth, float DepthClearValue, bool ClearStencil, uint8 StencilClearValue) override;

		virtual void EndRenderpass() override;

		virtual void SetViewports(uint32 ViewportCount, ViewportArea* Viewports) override;

		virtual void SetScissors(uint32 ScissorCount, ScissorArea* Scissors) override;

		virtual void BindGraphicPipeline(SharedPtr<RHIPipeline_Graphic> GraphicPipeline) override;

		virtual void BindVertexBuffers(uint32 VertexBufferCount, SharedPtr<RHIBuffer>* VertexBuffers, uint32* Offsets, uint32 FirstBinding = 0) override;

		virtual void BindIndexBuffer(SharedPtr<RHIBuffer> IndexBuffer, EIndexBufferType Type = EIndexBufferType::UInt32, uint32 Offset = 0) override;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<RHIBuffer> Buffer) override;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<RHIImage> Image) override;

		virtual void BindShaderResource(uint32 Binding, SharedPtr<Sampler> Sampler) override;

		virtual void DrawPrimitive(uint32 VertexCount, uint32 StartVertex = 0, uint32_t InstanceCount = 1, uint32 FirstInstance = 0) override;

		virtual void DrawIndexedPrimitive(uint32 IndexCount, uint32 FirstIndex = 0, uint32 StartVertex = 0, uint32 InstanceCount = 1, uint32 FirstInstance = 0) override;

		virtual void CopyImage(SharedPtr<RHIImage> From, SharedPtr<RHIImage> To) override;

		virtual void CopyBuffer(SharedPtr<RHIBuffer> From, SharedPtr<RHIBuffer> To) override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Context() const
		{
			return m_Context;
		}

	protected:
		DX11_DynamicRHI* m_Owner = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
	};
}
