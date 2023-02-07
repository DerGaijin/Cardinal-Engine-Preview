#include "DX11_RHIContextImmediate.h"
#include "DX11_DynamicRHI.h"
#include "DX11_Swapchain.h"
#include "DX11_RHIBuffer.h"
#include "DX11_RHIImage.h"

#include "DX11_Utilities.h"

namespace CE
{
	bool DX11_RHIContextImmediate::IsValid() const
	{
		return DX11_RHIContext::IsValid();
	}

	DynamicRHI* DX11_RHIContextImmediate::Owner() const
	{
		return DX11_RHIContext::Owner();
	}

	void DX11_RHIContextImmediate::Destroy()
	{
		DX11_RHIContext::Destroy();
	}

	void DX11_RHIContextImmediate::BeginRenderpass(const Array<SharedPtr<RHIImage>>& RenderTargets, const SharedPtr<RHIImage>& DepthTarget)
	{
		DX11_RHIContext::BeginRenderpass(RenderTargets, DepthTarget);
	}

	void DX11_RHIContextImmediate::ClearRenderTarget(const SharedPtr<RHIImage>& RenderTarget, const Color& ClearColor)
	{
		DX11_RHIContext::ClearRenderTarget(RenderTarget, ClearColor);
	}

	void DX11_RHIContextImmediate::ClearDepthTarget(const SharedPtr<RHIImage>& DepthTarget, bool ClearDepth, float DepthClearValue, bool ClearStencil, uint8 StencilClearValue)
	{
		DX11_RHIContext::ClearDepthTarget(DepthTarget, ClearDepth, DepthClearValue, ClearStencil, StencilClearValue);
	}

	void DX11_RHIContextImmediate::EndRenderpass()
	{
		DX11_RHIContext::EndRenderpass();
	}

	void DX11_RHIContextImmediate::SetViewports(uint32 ViewportCount, ViewportArea* Viewports)
	{
		DX11_RHIContext::SetViewports(ViewportCount, Viewports);
	}

	void DX11_RHIContextImmediate::SetScissors(uint32 ScissorCount, ScissorArea* Scissors)
	{
		DX11_RHIContext::SetScissors(ScissorCount, Scissors);
	}

	void DX11_RHIContextImmediate::BindGraphicPipeline(SharedPtr<RHIPipeline_Graphic> GraphicPipeline)
	{
		DX11_RHIContext::BindGraphicPipeline(GraphicPipeline);
	}

	void DX11_RHIContextImmediate::BindVertexBuffers(uint32 VertexBufferCount, SharedPtr<RHIBuffer>* VertexBuffers, uint32* Offsets, uint32 FirstBinding /*= 0*/)
	{
		DX11_RHIContext::BindVertexBuffers(VertexBufferCount, VertexBuffers, Offsets, FirstBinding);
	}

	void DX11_RHIContextImmediate::BindIndexBuffer(SharedPtr<RHIBuffer> IndexBuffer, EIndexBufferType Type /*= EIndexBufferType::UInt32*/, uint32 Offset /*= 0*/)
	{
		DX11_RHIContext::BindIndexBuffer(IndexBuffer, Type, Offset);
	}

	void DX11_RHIContextImmediate::BindShaderResource(uint32 Binding, SharedPtr<RHIBuffer> Buffer)
	{
		DX11_RHIContext::BindShaderResource(Binding, Buffer);
	}

	void DX11_RHIContextImmediate::BindShaderResource(uint32 Binding, SharedPtr<RHIImage> Image)
	{
		DX11_RHIContext::BindShaderResource(Binding, Image);
	}

	void DX11_RHIContextImmediate::BindShaderResource(uint32 Binding, SharedPtr<Sampler> Sampler)
	{
		DX11_RHIContext::BindShaderResource(Binding, Sampler);
	}

	void DX11_RHIContextImmediate::DrawPrimitive(uint32 VertexCount, uint32 StartVertex /*= 0*/, uint32_t InstanceCount /*= 1*/, uint32 FirstInstance /*= 0*/)
	{
		DX11_RHIContext::DrawPrimitive(VertexCount, StartVertex, InstanceCount, FirstInstance);
	}

	void DX11_RHIContextImmediate::DrawIndexedPrimitive(uint32 IndexCount, uint32 FirstIndex /*= 0*/, uint32 StartVertex /*= 0*/, uint32 InstanceCount /*= 1*/, uint32 FirstInstance /*= 0*/)
	{
		DX11_RHIContext::DrawIndexedPrimitive(IndexCount, FirstIndex, StartVertex, InstanceCount, FirstInstance);
	}

	void DX11_RHIContextImmediate::CopyImage(SharedPtr<RHIImage> From, SharedPtr<RHIImage> To)
	{
		DX11_RHIContext::CopyImage(From, To);
	}


	void DX11_RHIContextImmediate::CopyBuffer(SharedPtr<RHIBuffer> From, SharedPtr<RHIBuffer> To)
	{
		DX11_RHIContext::CopyBuffer(From, To);
	}

	void DX11_RHIContextImmediate::Flush()
	{
		m_Context->Flush();
	}

	void DX11_RHIContextImmediate::PrepareSwapchain(const SharedPtr<Swapchain>& Swapchain)
	{

	}

	void DX11_RHIContextImmediate::PresentSwapchain(const SharedPtr<Swapchain>& Swapchain)
	{
		DX11_Swapchain* Casted = RHIObject::Cast<DX11_Swapchain>(Swapchain.Get(), m_Owner);
		if (Casted != nullptr)
		{
			Casted->NativeSwapchain()->Present(0, 0);
		}
	}

	void* DX11_RHIContextImmediate::Map(const SharedPtr<RHIBuffer>& Buffer, ERHIMapAccess Access)
	{
		DX11_RHIBuffer* Casted = RHIObject::Cast<DX11_RHIBuffer>(Buffer.Get(), m_Owner);
		if (Casted != nullptr)
		{
			return GenericMap(Casted->NativeBuffer().Get(), Access, Buffer->Description.Access);
		}

		return nullptr;
	}

	void DX11_RHIContextImmediate::UnMap(const SharedPtr<RHIBuffer>& Buffer)
	{
		DX11_RHIBuffer* Casted = RHIObject::Cast<DX11_RHIBuffer>(Buffer.Get(), m_Owner);
		if (Casted != nullptr)
		{
			m_Context->Unmap(Casted->NativeBuffer().Get(), 0);
		}
	}

	void* DX11_RHIContextImmediate::Map(const SharedPtr<RHIImage>& Image, ERHIMapAccess Access)
	{
		DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(Image.Get(), m_Owner);
		if (Casted != nullptr)
		{
			return GenericMap(Casted->NativeImage().Get(), Access, Image->Description.Access);
		}

		return nullptr;
	}

	void DX11_RHIContextImmediate::UnMap(const SharedPtr<RHIImage>& Image)
	{
		DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(Image.Get(), m_Owner);
		if (Casted != nullptr)
		{
			m_Context->Unmap(Casted->NativeImage().Get(), 0);
		}
	}

	void* DX11_RHIContextImmediate::GenericMap(ID3D11Resource* Resource, ERHIMapAccess Access, ERHIAccess RHIAccess)
	{
		D3D11_MAP MapType = D3D11_MAP_READ;
		switch (Access)
		{
		case ERHIMapAccess::Read:
			MapType = D3D11_MAP_READ;
			break;
		case ERHIMapAccess::Write:
			MapType = RHIAccess == ERHIAccess::All ? D3D11_MAP_WRITE : D3D11_MAP_WRITE_DISCARD;
			break;
		case ERHIMapAccess::ReadWrite:
			MapType = D3D11_MAP_READ_WRITE;
			break;
		}

		D3D11_MAPPED_SUBRESOURCE MappedSubRes;
		HRESULT Result = m_Context->Map(Resource, 0, MapType, 0, &MappedSubRes);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Warning, "Failed to Map Resource (%s)", DX11_Utilities::ResultToString(Result));
			return nullptr;
		}
		return MappedSubRes.pData;
	}
}
