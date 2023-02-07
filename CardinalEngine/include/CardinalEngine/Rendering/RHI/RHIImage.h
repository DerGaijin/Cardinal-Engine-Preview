#pragma once
#include "../../Framework/Core/Headers.h"
#include "Enums.h"
#include "RHIObject.h"

namespace CE
{
	struct RHIImageDescription
	{
		ERHIImageUsage Usage = ERHIImageUsage::None;
		uint32 Width = 0;
		uint32 Height = 0;
		uint32 Depth = 1;
		EDataFormat Format = EDataFormat::RGBA8_UNorm;
		ERHIImageDimension Dimension = ERHIImageDimension::Texture_2D;
		ERHIAccess Access = ERHIAccess::GPUOnly;
		uint32 MipLevels = 1;
		ESampleCount Samples = ESampleCount::Count_1;
	};

	class CE_API RHIImage : public RHIObject
	{
	public:
		RHIImage(const RHIImageDescription& Description) : Description(Description) {}

	

	public:
		uint32 ByteSize() const;

	public:
		const RHIImageDescription Description;
	};
}
