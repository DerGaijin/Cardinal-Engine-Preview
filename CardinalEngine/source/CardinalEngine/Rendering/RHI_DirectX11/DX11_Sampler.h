#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/Sampler.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class DX11_Sampler : public Sampler
	{
	public:
		DX11_Sampler(DX11_DynamicRHI* Owner, const SamplerDescription& Description);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	public:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> NativeSampler() const
		{
			return m_Sampler;
		}

	private:
		DX11_DynamicRHI* m_Owner = nullptr;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler = nullptr;;
	};
}
