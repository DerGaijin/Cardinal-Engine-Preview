#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class RHIImage;

	class CE_API RenderTarget : public NonCopyable
	{
	public:
		virtual SharedPtr<RHIImage> GetTargetImage() const = 0;

	};
}
