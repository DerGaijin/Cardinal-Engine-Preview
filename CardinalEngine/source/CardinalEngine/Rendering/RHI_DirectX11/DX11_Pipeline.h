#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/Pipeline.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class DX11_RHIPipeline_Grpahic : public RHIPipeline_Graphic
	{
	public:
		DX11_RHIPipeline_Grpahic(DX11_DynamicRHI* Owner, const RHIPipeline_GraphicShaders& Shaders, const RHIPipeline_GraphicDescription& Description);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetInputLayout() const
		{
			return m_InputLayout;
		}

		const Microsoft::WRL::ComPtr<ID3D11RasterizerState>& GetRasterizerState() const
		{
			return m_RasterizerState;
		}

		const Microsoft::WRL::ComPtr<ID3D11BlendState>& GetBlendState() const
		{
			return m_BlendState;
		}

		const Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& GetDepthStencilState() const
		{
			return m_DepthStencilState;
		}

		const Microsoft::WRL::ComPtr<ID3D11VertexShader>& VertexShader() const
		{
			return m_VertexShader;
		}

		const Microsoft::WRL::ComPtr<ID3D11HullShader>& HullShader() const
		{
			return m_HullShader;
		}

		const Microsoft::WRL::ComPtr<ID3D11DomainShader>& DomainShader() const
		{
			return m_DomainShader;
		}

		const Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GeometryShader() const
		{
			return m_GeometryShader;
		}

		const Microsoft::WRL::ComPtr<ID3D11PixelShader>& PixelShader() const
		{
			return m_PixelShader;
		}

	private:
		DX11_DynamicRHI* m_Owner = nullptr;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState = nullptr;
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState = nullptr;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_HullShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_DomainShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;
	};
}
