#include "DX11_Pipeline.h"
#include "DX11_DynamicRHI.h"
#include "DX11_Shader.h"

#include "DX11_Utilities.h"

namespace CE
{
	DX11_RHIPipeline_Grpahic::DX11_RHIPipeline_Grpahic(DX11_DynamicRHI* Owner, const RHIPipeline_GraphicShaders& Shaders, const RHIPipeline_GraphicDescription& InDescription)
		: m_Owner(Owner), RHIPipeline_Graphic(InDescription)
	{
		DX11_VertexShader* VertexShader = RHIObject::Cast<DX11_VertexShader>(Shaders.VertexShader.Get(), m_Owner);
		if (VertexShader == nullptr)
		{
			CE_LOG(ELog::Error, "Failed to create Graphic Pipline because the Vertex Shader is invalid");
			return;
		}

		m_VertexShader = VertexShader->NativeShader();
		Microsoft::WRL::ComPtr<ID3DBlob> VertexShaderByteCode = VertexShader->NativeShaderByteCode();
		if (VertexShaderByteCode == nullptr)
		{
			CE_LOG(ELog::Error, "Failed to create Graphic Pipeline (Missing Vertex Shader Byte Code)");
			return;
		}

		DX11_FragmentShader* FragmentShader = RHIObject::Cast<DX11_FragmentShader>(Shaders.FragmentShader.Get(), m_Owner);
		if (FragmentShader == nullptr)
		{
			CE_LOG(ELog::Error, "Failed to create Graphic Pipline because the Fragment Shader is invalid");
			return;
		}

		m_PixelShader = FragmentShader->NativeShader();

		DX11_TessellationControlShader* TessellationControlShader = RHIObject::Cast<DX11_TessellationControlShader>(Shaders.TessellationControlShader.Get(), m_Owner);
		if (TessellationControlShader != nullptr)
		{
			m_HullShader = TessellationControlShader->NativeShader();
		}

		DX11_TessellationEvaluationShader* TessellationEvaluationShader = RHIObject::Cast<DX11_TessellationEvaluationShader>(Shaders.TessellationEvaluationShader.Get(), m_Owner);
		if (TessellationEvaluationShader != nullptr)
		{
			m_DomainShader = TessellationEvaluationShader->NativeShader();
		}

		DX11_GeometryShader* GeometryShader = RHIObject::Cast<DX11_GeometryShader>(Shaders.GeometryShader.Get(), m_Owner);
		if (GeometryShader != nullptr)
		{
			m_GeometryShader = GeometryShader->NativeShader();
		}

		// Create the Input Layout
		Array<D3D11_INPUT_ELEMENT_DESC> InputElements;
		InputElements.Reserve(Description.VertexInputLayout.Size());
		Map<uint32_t, uint32_t> Offsets;
		for (auto& Element : Description.VertexInputLayout)
		{
			auto& InputElement = InputElements.EmplaceRef();
			InputElement.SemanticName = "ATTRIB";
			InputElement.SemanticIndex = Element.Index;
			InputElement.Format = DX11_Utilities::TranslateDataFormat(Element.Format);
			InputElement.InputSlot = Element.BufferIndex;
			InputElement.AlignedByteOffset = Offsets[Element.BufferIndex] + Element.RelativeOffset;
			InputElement.InputSlotClass = Element.PerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			InputElement.InstanceDataStepRate = Element.PerInstance ? 1 : 0;
			Offsets[Element.BufferIndex] += DynamicRHI::SizeOfDataFormat(Element.Format) + Element.RelativeOffset;
		}

		HRESULT Result = S_OK;
		if (InputElements.Size() > 0)
		{
			Result = m_Owner->Device()->CreateInputLayout(InputElements.Data(), (UINT)InputElements.Size(), VertexShaderByteCode->GetBufferPointer(), VertexShaderByteCode->GetBufferSize(), &m_InputLayout);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Input Layout (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
		}

		// Create The Rasterizer State
		D3D11_RASTERIZER_DESC RasterizerDesc{};
		RasterizerDesc.FillMode = Description.Rasterizer.WireFrame ? D3D11_FILL_MODE::D3D11_FILL_WIREFRAME : D3D11_FILL_MODE::D3D11_FILL_SOLID;
		switch (Description.Rasterizer.CullMode)
		{
		case ECullMode::None:
			RasterizerDesc.CullMode = D3D11_CULL_NONE;
			break;
		default:
		case ECullMode::Back:
			RasterizerDesc.CullMode = D3D11_CULL_BACK;
			break;
		case ECullMode::Front:
			RasterizerDesc.CullMode = D3D11_CULL_FRONT;
			break;
		}
		RasterizerDesc.FrontCounterClockwise = Description.Rasterizer.FrontIsCounterClockwise;
		RasterizerDesc.DepthBias = Description.Rasterizer.DepthBias;
		RasterizerDesc.DepthBiasClamp = Description.Rasterizer.DepthBiasClamp;
		RasterizerDesc.SlopeScaledDepthBias = Description.Rasterizer.DepthBiasSlopeFactor;
		RasterizerDesc.DepthClipEnable = Description.Rasterizer.DepthClampEnabled;
		RasterizerDesc.ScissorEnable = Description.Rasterizer.ScissorEnabled;
		RasterizerDesc.MultisampleEnable = FALSE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;
		Result = m_Owner->Device()->CreateRasterizerState(&RasterizerDesc, &m_RasterizerState);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Rasterizer State (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}

		static auto TranslateBlendOp = [](EBlendOperation Op) -> D3D11_BLEND_OP
		{
			switch (Op)
			{
			default:
			case CE::EBlendOperation::Undefined:
				return (D3D11_BLEND_OP)0;
			case CE::EBlendOperation::Add:
				return D3D11_BLEND_OP_ADD;
			case CE::EBlendOperation::Subtract:
				return D3D11_BLEND_OP_SUBTRACT;
			case CE::EBlendOperation::RevSubtract:
				return D3D11_BLEND_OP_REV_SUBTRACT;
			case CE::EBlendOperation::Min:
				return D3D11_BLEND_OP_MIN;
			case CE::EBlendOperation::Max:
				return D3D11_BLEND_OP_MAX;
			}
		};

		static auto TranslateBlendFactor = [](EBlendFactor Factor) -> D3D11_BLEND
		{
			switch (Factor)
			{
			default:
			case CE::EBlendFactor::Undefined:
				return (D3D11_BLEND)0;
			case CE::EBlendFactor::Zero:
				return D3D11_BLEND_ZERO;
			case CE::EBlendFactor::One:
				return D3D11_BLEND_ONE;
			case CE::EBlendFactor::SrcColor:
				return D3D11_BLEND_SRC_COLOR;
			case CE::EBlendFactor::InvSrcColor:
				return D3D11_BLEND_INV_SRC_COLOR;
			case CE::EBlendFactor::SrcAlpha:
				return D3D11_BLEND_SRC_ALPHA;
			case CE::EBlendFactor::InvSrcAlpha:
				return D3D11_BLEND_INV_SRC_ALPHA;
			case CE::EBlendFactor::DstAlpha:
				return D3D11_BLEND_DEST_ALPHA;
			case CE::EBlendFactor::InvDstAlpha:
				return D3D11_BLEND_INV_DEST_ALPHA;
			case CE::EBlendFactor::DstColor:
				return D3D11_BLEND_DEST_COLOR;
			case CE::EBlendFactor::InvDstColor:
				return D3D11_BLEND_INV_DEST_COLOR;
			case CE::EBlendFactor::SrcAlphaSat:
				return D3D11_BLEND_SRC_ALPHA_SAT;
			case CE::EBlendFactor::BlendFactor:
				return D3D11_BLEND_BLEND_FACTOR;
			case CE::EBlendFactor::InvBlendFactor:
				return D3D11_BLEND_INV_BLEND_FACTOR;
			case CE::EBlendFactor::Src1Color:
				return D3D11_BLEND_SRC1_COLOR;
			case CE::EBlendFactor::InvSrc1Color:
				return D3D11_BLEND_INV_SRC1_COLOR;
			case CE::EBlendFactor::Src1Alpha:
				return D3D11_BLEND_SRC1_ALPHA;
			case CE::EBlendFactor::InvSrc1Alpha:
				return D3D11_BLEND_INV_SRC1_ALPHA;
			}
		};

		// Create the Blend State
		D3D11_BLEND_DESC BlendDesc{};
		BlendDesc.AlphaToCoverageEnable = Description.BlendState.AlphaToCoverageEnabled;
		BlendDesc.IndependentBlendEnable = Description.BlendState.IndependetBlendEnabled;
		for (uint8_t i = 0; i < 8; i++)
		{
			if (Description.BlendState.RenderTargetBlends.Size() <= i)
				break;
			auto& Current = Description.BlendState.RenderTargetBlends[i];
			BlendDesc.RenderTarget[i].BlendEnable = Current.BlendEnabled;
			BlendDesc.RenderTarget[i].BlendOp = TranslateBlendOp(Current.BlendOp);
			BlendDesc.RenderTarget[i].BlendOpAlpha = TranslateBlendOp(Current.BlendOpAlpha);
			BlendDesc.RenderTarget[i].DestBlend = TranslateBlendFactor(Current.DstBlend);
			BlendDesc.RenderTarget[i].DestBlendAlpha = TranslateBlendFactor(Current.DstBlendAlpha);
			if (Current.WriteMask & EBlendMask::Component_R)
				BlendDesc.RenderTarget[i].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
			if (Current.WriteMask & EBlendMask::Component_G)
				BlendDesc.RenderTarget[i].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
			if (Current.WriteMask & EBlendMask::Component_B)
				BlendDesc.RenderTarget[i].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
			if (Current.WriteMask & EBlendMask::Component_A)
				BlendDesc.RenderTarget[i].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
			BlendDesc.RenderTarget[i].SrcBlend = TranslateBlendFactor(Current.SrcBlend);
			BlendDesc.RenderTarget[i].SrcBlendAlpha = TranslateBlendFactor(Current.SrcBlendAlpha);
		}

		if (Description.BlendState.RenderTargetBlends.Size() > 8)
		{
			CE_LOG(ELog::Warning, "More then 8 Blend states provided, ignoring all over index 7");
		}

		if (Description.BlendState.RenderTargetBlends.Size() > 0)
		{
			Result = m_Owner->Device()->CreateBlendState(&BlendDesc, &m_BlendState);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Blend State (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
		}

		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{};
		DepthStencilDesc.DepthEnable = Description.DepthStencilState.DepthEnabled;
		DepthStencilDesc.DepthWriteMask = Description.DepthStencilState.DepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.DepthFunc = DX11_Utilities::TranslateCompareOp(Description.DepthStencilState.DepthCompare);
		DepthStencilDesc.StencilEnable = Description.DepthStencilState.StencilEnabled;
		DepthStencilDesc.StencilReadMask = Description.DepthStencilState.ReadMaskAll ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.StencilWriteMask = Description.DepthStencilState.WriteMaskAll ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.FrontFace.StencilDepthFailOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.FrontStencilDepthFailOp);
		DepthStencilDesc.FrontFace.StencilFailOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.FrontStencilFailOp);
		DepthStencilDesc.FrontFace.StencilFunc = DX11_Utilities::TranslateCompareOp(Description.DepthStencilState.FrontStencilOp);
		DepthStencilDesc.FrontFace.StencilPassOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.FrontStencilPassOp);
		DepthStencilDesc.BackFace.StencilDepthFailOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.BackStencilDepthFailOp);
		DepthStencilDesc.BackFace.StencilFailOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.BackStencilFailOp);
		DepthStencilDesc.BackFace.StencilFunc = DX11_Utilities::TranslateCompareOp(Description.DepthStencilState.BackStencilOp);
		DepthStencilDesc.BackFace.StencilPassOp = DX11_Utilities::TranslateStencilOp(Description.DepthStencilState.BackStencilPassOp);

		Result = m_Owner->Device()->CreateDepthStencilState(&DepthStencilDesc, &m_DepthStencilState);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Depth Stencil State (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_RHIPipeline_Grpahic::IsValid() const
	{
		return m_VertexShader != nullptr && m_PixelShader != nullptr && m_RasterizerState != nullptr;
	}

	DynamicRHI* DX11_RHIPipeline_Grpahic::Owner() const
	{
		return m_Owner;
	}

	void DX11_RHIPipeline_Grpahic::Destroy()
	{
		m_VertexShader = nullptr;
		m_HullShader = nullptr;
		m_DomainShader = nullptr;
		m_GeometryShader = nullptr;
		m_PixelShader = nullptr;
		m_InputLayout = nullptr;
		m_RasterizerState = nullptr;
		m_BlendState = nullptr;
		m_DepthStencilState = nullptr;
	}
}
