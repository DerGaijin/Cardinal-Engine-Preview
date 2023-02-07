#include "CardinalEngine/Material/Texture.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Framework/Serialization/Archive.h"
#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"

namespace CE
{
	void Texture::SerializeAsset(Archive& Ar)
	{
		RHIImageDescription ImageDesc;
		if (Ar.IsSaving())
		{
			ImageDesc = m_Image->Description;
		}

		Ar << ImageDesc.Usage;
		Ar << ImageDesc.Width;
		Ar << ImageDesc.Height;
		Ar << ImageDesc.Depth;
		Ar << ImageDesc.Format;
		Ar << ImageDesc.Dimension;
		Ar << ImageDesc.Access;
		Ar << ImageDesc.MipLevels;
		Ar << ImageDesc.Samples;

		if (Ar.IsLoading())
		{
			m_Image = Engine::Get().DynamicRHI().CreateImage(ImageDesc);
		}

		ImageDesc.Access = ERHIAccess::All;

		SharedPtr<RHIImage> Staging = Engine::Get().DynamicRHI().CreateImage(ImageDesc);

		Array<signed char> ImageBinary;
		if (Ar.IsLoading())
		{
			Ar << ImageBinary;
		}
		else
		{
			ImageBinary.AddUninitialized(Staging->ByteSize());
		}

		auto SerializeTask = RenderThread::Enqueue([&](RHIContextImmediate& Context)
			{
				if (Ar.IsLoading())
				{
					void* Mapped = Context.Map(Staging, ERHIMapAccess::Write);
					if (Mapped == nullptr)
					{
						return;
					}

					memcpy(Mapped, ImageBinary.Data(), Staging->ByteSize());

					Context.UnMap(Staging);

					Context.CopyImage(Staging, m_Image);
				}
				else
				{
					Context.CopyImage(m_Image, Staging);

					//@TODO: Maybe wait for Copy to complete ?

					void* Mapped = Context.Map(Staging, ERHIMapAccess::Write);
					if (Mapped == nullptr)
					{
						return;
					}

					memcpy(ImageBinary.Data(), Mapped, Staging->ByteSize());

					Context.UnMap(Staging);
				}
			});

		SerializeTask->Wait();

		if (Ar.IsSaving())
		{
			Ar << ImageBinary;
		}
	}
}
