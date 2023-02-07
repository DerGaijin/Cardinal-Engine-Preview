#include "DX11_RHIContext.h"
#include "DX11_DynamicRHI.h"
#include "DX11_RHIImage.h"
#include "DX11_RHIBuffer.h"
#include "DX11_Pipeline.h"
#include "DX11_Sampler.h"

namespace CE
{
	DX11_RHIContext::DX11_RHIContext(DX11_DynamicRHI* Owner, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Context) : m_Owner(Owner), m_Context(Context)
	{

	}

	bool DX11_RHIContext::IsValid() const
	{
		return m_Context != nullptr;
	}

	DynamicRHI* DX11_RHIContext::Owner() const
	{
		return m_Owner;
	}

	void DX11_RHIContext::Destroy()
	{
		m_Context = nullptr;
	}

	void DX11_RHIContext::BeginRenderpass(const Array<SharedPtr<RHIImage>>& RenderTargets, const SharedPtr<RHIImage>& DepthTarget)
	{
		Array<ID3D11RenderTargetView*> NativeRTVs;
		if (RenderTargets.Size() > 0)
		{
			NativeRTVs.AddZeroed(RenderTargets.Size());
			NativeRTVs.Reserve(RenderTargets.Size());
		}
		ID3D11DepthStencilView* NativeDSV = nullptr;

		uint32 RTVIndex = 0;
		for (auto& RT : RenderTargets)
		{
			DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(RT.Get(), m_Owner);
			if (Casted != nullptr)
			{
				NativeRTVs[RTVIndex] = Casted->GetRenderTargetView().Get();
			}

			RTVIndex++;
		}

		if (DepthTarget != nullptr)
		{
			DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(DepthTarget.Get(), m_Owner);
			if (Casted != nullptr)
			{
				NativeDSV = Casted->GetDepthStencilView().Get();
			}
		}

		m_Context->OMSetRenderTargets(NativeRTVs.Size(), NativeRTVs.Data(), NativeDSV);
	}

	void DX11_RHIContext::ClearRenderTarget(const SharedPtr<RHIImage>& RenderTarget, const Color& ClearColor)
	{
		DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(RenderTarget.Get(), m_Owner);
		if (Casted == nullptr)
		{
			return;
		}

		m_Context->ClearRenderTargetView(Casted->GetRenderTargetView().Get(), &ClearColor.R);
	}

	void DX11_RHIContext::ClearDepthTarget(const SharedPtr<RHIImage>& DepthTarget, bool ClearDepth, float DepthClearValue, bool ClearStencil, uint8 StencilClearValue)
	{
		DX11_RHIImage* Casted = RHIObject::Cast<DX11_RHIImage>(DepthTarget.Get(), m_Owner);
		if (Casted == nullptr)
		{
			return;
		}

		uint32 ClearFlags = 0;
		if (ClearDepth)
		{
			ClearFlags |= D3D11_CLEAR_DEPTH;
		}
		if (ClearStencil)
		{
			ClearFlags |= D3D11_CLEAR_STENCIL;
		}

		m_Context->ClearDepthStencilView(Casted->GetDepthStencilView().Get(), ClearFlags, DepthClearValue, StencilClearValue);
	}

	void DX11_RHIContext::EndRenderpass()
	{
		m_Context->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void DX11_RHIContext::SetViewports(uint32 ViewportCount, ViewportArea* Viewports)
	{
		Array<D3D11_VIEWPORT> D3Viewports;
		D3Viewports.Reserve(ViewportCount);

		for (auto It = Viewports; It != Viewports + ViewportCount; It++)
		{
			D3D11_VIEWPORT& D3Viewport = D3Viewports.EmplaceRef();
			D3Viewport.TopLeftX = It->X;
			D3Viewport.TopLeftY = It->Y;
			D3Viewport.Width = It->Width;
			D3Viewport.Height = It->Height;
			D3Viewport.MinDepth = It->MinDepth;
			D3Viewport.MaxDepth = It->MaxDepth;
		}

		if (D3Viewports.Size() > 0)
		{
			m_Context->RSSetViewports((uint32_t)D3Viewports.Size(), D3Viewports.Data());
		}
		else
		{
			m_Context->RSSetViewports(0, nullptr);
		}
	}

	void DX11_RHIContext::SetScissors(uint32 ScissorCount, ScissorArea* Scissors)
	{
		Array<D3D11_RECT> D3Scissors;
		D3Scissors.Reserve(ScissorCount);

		for (auto It = Scissors; It != Scissors + ScissorCount; It++)
		{
			D3D11_RECT& D3Scissor = D3Scissors.EmplaceRef();
			D3Scissor.left = It->OffsetX;
			D3Scissor.right = It->OffsetX + It->ExtentX;
			D3Scissor.top = It->OffsetY;
			D3Scissor.bottom = It->OffsetY + It->ExtentY;
		}

		if (D3Scissors.Size() > 0)
		{
			m_Context->RSSetScissorRects((uint32_t)D3Scissors.Size(), D3Scissors.Data());
		}
		else
		{
			m_Context->RSSetScissorRects(0, nullptr);
		}
	}

	void DX11_RHIContext::BindGraphicPipeline(SharedPtr<RHIPipeline_Graphic> GraphicPipeline)
	{
		DX11_RHIPipeline_Grpahic* Casted = RHIObject::Cast<DX11_RHIPipeline_Grpahic>(GraphicPipeline.Get(), m_Owner);
		if (GraphicPipeline == nullptr || !GraphicPipeline->IsValid() || GraphicPipeline->Owner() != m_Owner)
		{
			CE_LOG(ELog::Warning, "Could not Bind Graphic Pipeline (Invalid GraphicPipeline)");
			return;
		}

		m_Context->VSSetShader(Casted->VertexShader().Get(), nullptr, 0);
		m_Context->HSSetShader(Casted->HullShader().Get(), nullptr, 0);
		m_Context->DSSetShader(Casted->DomainShader().Get(), nullptr, 0);
		m_Context->GSSetShader(Casted->GeometryShader().Get(), nullptr, 0);
		m_Context->PSSetShader(Casted->PixelShader().Get(), nullptr, 0);

		D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		switch (Casted->Description.Topology)
		{
		default:
		case CE::EPrimitiveTopology::TriangleList:
			Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case CE::EPrimitiveTopology::TriangleStrip:
			Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case CE::EPrimitiveTopology::PointList:
			Topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case CE::EPrimitiveTopology::LineList:
			Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case CE::EPrimitiveTopology::LineStrip:
			Topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		}
		m_Context->IASetPrimitiveTopology(Topology);

		m_Context->IASetInputLayout(Casted->GetInputLayout().Get());
		m_Context->RSSetState(Casted->GetRasterizerState().Get());
		float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_Context->OMSetBlendState(Casted->GetBlendState().Get(), BlendFactor, 0xf);
		m_Context->OMSetDepthStencilState(Casted->GetDepthStencilState().Get(), 0);
	}

	void DX11_RHIContext::BindVertexBuffers(uint32 VertexBufferCount, SharedPtr<RHIBuffer>* VertexBuffers, uint32* Offsets, uint32 FirstBinding /*= 0*/)
	{
		Array<ID3D11Buffer*> D3VertexBuffers;
		Array<UINT> D3Offsets;
		Array<UINT> D3Strides;
		D3VertexBuffers.Reserve(VertexBufferCount);
		D3Offsets.Reserve(VertexBufferCount);
		D3Strides.Reserve(VertexBufferCount);

		for (size_t i = 0; i < VertexBufferCount; i++)
		{
			RHIBuffer* Current = VertexBuffers[i].Get();
			if (Current == nullptr || !Current->IsValid() || Current->Owner() != m_Owner || Current->Description.Usage != ERHIBufferUsage::VertexBuffer)
			{
				CE_LOG(ELog::Warning, "Could not bind Vertex Buffer[%i] (Invalid Vertex Buffer)", i);
				continue;
			}

			DX11_RHIBuffer* CastedVertexBuffer = static_cast<DX11_RHIBuffer*>(Current);
			D3VertexBuffers.Emplace(CastedVertexBuffer->NativeBuffer().Get());
			D3Offsets.Emplace(Offsets[i]);
			D3Strides.Emplace((UINT)CastedVertexBuffer->Description.ElementSize);
		}

		m_Context->IASetVertexBuffers(FirstBinding, (uint32_t)D3VertexBuffers.Size(), D3VertexBuffers.Data(), D3Strides.Data(), D3Offsets.Data());
	}

	void DX11_RHIContext::BindIndexBuffer(SharedPtr<RHIBuffer> IndexBuffer, EIndexBufferType Type /*= EIndexBufferType::UInt32*/, uint32 Offset /*= 0*/)
	{
		DX11_RHIBuffer* CastedBuffer = RHIObject::Cast<DX11_RHIBuffer>(IndexBuffer.Get(), m_Owner);
		if (CastedBuffer == nullptr || CastedBuffer->Description.Usage != ERHIBufferUsage::IndexBuffer)
		{
			CE_LOG(ELog::Warning, "Could not bind Index buffer (Invalid IndexBuffer)");
			return;
		}

		m_Context->IASetIndexBuffer(CastedBuffer->NativeBuffer().Get(), Type == EIndexBufferType::UInt32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, Offset);
	}

	void DX11_RHIContext::BindShaderResource(uint32 Binding, SharedPtr<RHIBuffer> Buffer)
	{
		DX11_RHIBuffer* CastedBuffer = RHIObject::Cast<DX11_RHIBuffer>(Buffer.Get(), m_Owner);
		if (CastedBuffer == nullptr)
		{
			CE_LOG(ELog::Warning, "Failed to Bind Uniform Buffer at binding %i because the buffer is invalid", Binding);
			return;
		}

		ID3D11Buffer* NativeBuffer = CastedBuffer->NativeBuffer().Get();

		m_Context->VSSetConstantBuffers(Binding, 1, &NativeBuffer);
		m_Context->HSSetConstantBuffers(Binding, 1, &NativeBuffer);
		m_Context->DSSetConstantBuffers(Binding, 1, &NativeBuffer);
		m_Context->GSSetConstantBuffers(Binding, 1, &NativeBuffer);
		m_Context->PSSetConstantBuffers(Binding, 1, &NativeBuffer);
	}

	void DX11_RHIContext::BindShaderResource(uint32 Binding, SharedPtr<RHIImage> Image)
	{
		DX11_RHIImage* CastedImage = RHIObject::Cast<DX11_RHIImage>(Image.Get(), m_Owner);
		if (CastedImage == nullptr)
		{
			CE_LOG(ELog::Warning, "Failed to Bind Image at binding %i because the Image is invalid", Binding);
			return;
		}

		ID3D11ShaderResourceView* View = CastedImage->GetShaderResourceView().Get();

		m_Context->VSSetShaderResources(Binding, 1, &View);
		m_Context->HSSetShaderResources(Binding, 1, &View);
		m_Context->DSSetShaderResources(Binding, 1, &View);
		m_Context->GSSetShaderResources(Binding, 1, &View);
		m_Context->PSSetShaderResources(Binding, 1, &View);
	}

	void DX11_RHIContext::BindShaderResource(uint32 Binding, SharedPtr<Sampler> Sampler)
	{
		DX11_Sampler* CastedSampler = RHIObject::Cast<DX11_Sampler>(Sampler.Get(), m_Owner);
		if (CastedSampler == nullptr)
		{
			CE_LOG(ELog::Warning, "Failed to Bind Sampler at binding %i because the Sampler is invalid", Binding);
			return;
		}

		ID3D11SamplerState* SamplerState = CastedSampler->NativeSampler().Get();

		m_Context->VSSetSamplers(Binding, 1, &SamplerState);
		m_Context->HSSetSamplers(Binding, 1, &SamplerState);
		m_Context->DSSetSamplers(Binding, 1, &SamplerState);
		m_Context->GSSetSamplers(Binding, 1, &SamplerState);
		m_Context->PSSetSamplers(Binding, 1, &SamplerState);
	}

	void DX11_RHIContext::DrawPrimitive(uint32 VertexCount, uint32 StartVertex /*= 0*/, uint32_t InstanceCount /*= 1*/, uint32 FirstInstance /*= 0*/)
	{
		m_Context->DrawInstanced(VertexCount, InstanceCount, StartVertex, FirstInstance);
	}

	void DX11_RHIContext::DrawIndexedPrimitive(uint32 IndexCount, uint32 FirstIndex /*= 0*/, uint32 StartVertex /*= 0*/, uint32 InstanceCount /*= 1*/, uint32 FirstInstance /*= 0*/)
	{
		m_Context->DrawIndexedInstanced(IndexCount, InstanceCount, FirstIndex, StartVertex, FirstInstance);
	}

	void DX11_RHIContext::CopyImage(SharedPtr<RHIImage> From, SharedPtr<RHIImage> To)
	{
		DX11_RHIImage* CastedFrom = RHIObject::Cast<DX11_RHIImage>(From.Get(), m_Owner);
		DX11_RHIImage* CastedTo = RHIObject::Cast<DX11_RHIImage>(To.Get(), m_Owner);
		if (CastedFrom == nullptr || CastedTo == nullptr)
		{
			CE_LOG(ELog::Warning, "Failed to Copy Images because a Image is invalid");
			return;
		}

		m_Context->CopyResource(CastedTo->NativeImage().Get(), CastedFrom->NativeImage().Get());
	}

	void DX11_RHIContext::CopyBuffer(SharedPtr<RHIBuffer> From, SharedPtr<RHIBuffer> To)
	{
		DX11_RHIBuffer* CastedFrom = RHIObject::Cast<DX11_RHIBuffer>(From.Get(), m_Owner);
		DX11_RHIBuffer* CastedTo = RHIObject::Cast<DX11_RHIBuffer>(To.Get(), m_Owner);
		if (CastedFrom == nullptr || CastedTo == nullptr)
		{
			CE_LOG(ELog::Warning, "Failed to Copy Images because a Image is invalid");
			return;
		}

		m_Context->CopyResource(CastedTo->NativeBuffer().Get(), CastedFrom->NativeBuffer().Get());
	}
}
