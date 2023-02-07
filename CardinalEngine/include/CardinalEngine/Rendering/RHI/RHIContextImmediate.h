#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIContext.h"

namespace CE
{
	class Swapchain;
	class RHIBuffer;

	class CE_API RHIContextImmediate : public RHIContext
	{
	public:
		virtual bool IsDeferred() const override
		{
			return false;
		}

	public:
		virtual void Flush() = 0;

		virtual void PrepareSwapchain(const SharedPtr<Swapchain>& Swapchain) = 0;

		virtual void PresentSwapchain(const SharedPtr<Swapchain>& Swapchain) = 0;

		virtual void* Map(const SharedPtr<RHIBuffer>& Buffer, ERHIMapAccess Access) = 0;

		virtual void UnMap(const SharedPtr<RHIBuffer>& Buffer) = 0;

		virtual void* Map(const SharedPtr<RHIImage>& Image, ERHIMapAccess Access) = 0;

		virtual void UnMap(const SharedPtr<RHIImage>& Image) = 0;

	};
}
