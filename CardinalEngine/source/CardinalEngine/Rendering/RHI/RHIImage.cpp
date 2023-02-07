#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"

namespace CE
{
	uint32 RHIImage::ByteSize() const
	{
		return Description.Width * Description.Height * Description.Depth * DynamicRHI::SizeOfDataFormat(Description.Format);
	}
}
