#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIObject.h"
#include "Enums.h"

namespace CE
{
	struct RHIBufferDescription
	{
		uint32 Size = 0;
		uint32 ElementSize = 0;
		ERHIBufferUsage Usage = ERHIBufferUsage::None;
		ERHIAccess Access = ERHIAccess::GPUOnly;
	};

	class CE_API RHIBuffer : public RHIObject
	{
	public:
		RHIBuffer(const RHIBufferDescription& Description) : Description(Description) {}


	public:
		const RHIBufferDescription Description;
	};
}
