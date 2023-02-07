#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIObject.h"
#include "../RenderTarget.h"

namespace CE
{
	class RHIImage;

	class CE_API Swapchain : public RHIObject, public RenderTarget
	{
	public:

		virtual void Resize(uint32 Width, uint32 Height) = 0;

		virtual uint32 ImageCount() = 0;

		virtual SharedPtr<RHIImage> Image(uint32 Index) const = 0;

		virtual uint32 CurrentImageIndex() const = 0;

		inline SharedPtr<RHIImage> CurrentImage() const
		{
			return Image(CurrentImageIndex());
		}

		virtual SharedPtr<RHIImage> GetTargetImage() const override
		{
			return CurrentImage();
		}
	};
}
