#include "DX11_RHIBuffer.h"
#include "DX11_DynamicRHI.h"

#include "DX11_Utilities.h"

namespace CE
{
	DX11_RHIBuffer::DX11_RHIBuffer(DX11_DynamicRHI* Owner, const RHIBufferDescription& InDesc) : m_Owner(Owner), RHIBuffer(InDesc)
	{
		D3D11_BUFFER_DESC Desc{};
		Desc.ByteWidth = Description.Size;

		if (Description.ElementSize == 0)
		{
			CE_LOG(ELog::Error, "Failed to Create RHIBuffer because ElementSize is 0");
			return;
		}

		if (Desc.ByteWidth == 0)
		{
			CE_LOG(ELog::Error, "Failed to Create RHIBuffer because Size is 0");
			return;
		}
		else if (Desc.ByteWidth % Description.ElementSize != 0)
		{
			CE_LOG(ELog::Error, "Failed to Create RHIBuffer because Size is not dividable by ElementSize");
			return;
		}

		Desc.CPUAccessFlags = 0;
		switch (Description.Access)
		{
		case ERHIAccess::All:
			Desc.Usage = D3D11_USAGE_STAGING;
			Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;
		case ERHIAccess::GPUOnly:
			Desc.Usage = D3D11_USAGE_DEFAULT;
			Desc.CPUAccessFlags = 0;
			break;
		case ERHIAccess::GPURead_CPUWrite:
			Desc.Usage = D3D11_USAGE_DYNAMIC;
			Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		}

		Desc.BindFlags = 0;
		if (Description.Access != ERHIAccess::All)
		{
			switch (Description.Usage)
			{
			case ERHIBufferUsage::VertexBuffer:
				Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				break;
			case ERHIBufferUsage::IndexBuffer:
				Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				break;
			case ERHIBufferUsage::UniformBuffer:
				Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				break;
			}
		}


		Desc.MiscFlags = 0;
		Desc.StructureByteStride = Description.ElementSize;

		HRESULT Result = m_Owner->Device()->CreateBuffer(&Desc, nullptr, &m_Buffer);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Buffer (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_RHIBuffer::IsValid() const
	{
		return m_Buffer != nullptr;
	}

	DynamicRHI* DX11_RHIBuffer::Owner() const
	{
		return m_Owner;
	}

	void DX11_RHIBuffer::Destroy()
	{
		m_Buffer = nullptr;
	}
}
