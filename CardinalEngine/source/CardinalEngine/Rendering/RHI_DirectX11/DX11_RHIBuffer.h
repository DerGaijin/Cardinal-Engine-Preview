#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class DX11_RHIBuffer : public RHIBuffer
	{
	public:
		DX11_RHIBuffer(DX11_DynamicRHI* Owner, const RHIBufferDescription& Desc);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11Buffer>& NativeBuffer() const
		{
			return m_Buffer;
		}

	private:
		DX11_DynamicRHI* m_Owner = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer = nullptr;
	};
}
