#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIObject.h"
#include "Enums.h"

namespace CE
{
	struct SamplerDescription
	{
		ESamplerFilter MagFilter = ESamplerFilter::Linear;
		ESamplerFilter MinFilter = ESamplerFilter::Linear;
		ESamplerFilter MipMapMode = ESamplerFilter::Linear;
		ESamplerAddressMode AddressModeU = ESamplerAddressMode::Repeat;
		ESamplerAddressMode AddressModeV = ESamplerAddressMode::Repeat;
		ESamplerAddressMode AddressModeW = ESamplerAddressMode::Repeat;
		float MipLODBias = 0.0f;
		float MaxAnisotropy = 0.0f;
		bool EnableCompare = false;
		ECompareOp CompareOp = ECompareOp::Never;
		float MinLOD = 0.0f;
		float MaxLOD = 1.0f;
		ESamplerBorder Border = ESamplerBorder::OpaqueBlack;
	};

	class CE_API Sampler : public RHIObject
	{
	public:
		Sampler(const SamplerDescription& Description) : Description(Description) {}


	public:
		const SamplerDescription Description;
	};
}
