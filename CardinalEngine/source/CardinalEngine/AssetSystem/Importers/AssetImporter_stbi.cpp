#include "CardinalEngine/AssetSystem/Importers/AssetImporter_stbi.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Material/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace CE
{
	Array<String> AssetImporter_stbi::Extensions() const
	{
		return { "jpg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic" };
	}

	void AssetImporter_stbi::Import(const SharedPtr<AssetImporting>& Import)
	{
		int Width = 0, Height = 0, ComponentCount = 4;
		stbi_uc* TextureData = stbi_load(Import->Path.u8string().c_str(), &Width, &Height, &ComponentCount, STBI_rgb_alpha);
		if (TextureData == nullptr)
		{
			Import->State.set_value(EAssetImportResult::FileNotFound);
			return;
		}

		if (Width <= 0 || Height <= 0)
		{
			Import->State.set_value(EAssetImportResult::Failed);
			stbi_image_free(TextureData);
			return;
		}

		RHIImageDescription ImageDesc;
		ImageDesc.Usage = ERHIImageUsage::ShaderResource;
		ImageDesc.Width = Width;
		ImageDesc.Height = Height;
		ImageDesc.Access = ERHIAccess::All;

		SharedPtr<RHIImage> StagingImage = Engine::Get().DynamicRHI().CreateImage(ImageDesc);
		if (StagingImage == nullptr)
		{
			Import->State.set_value(EAssetImportResult::Failed);
			stbi_image_free(TextureData);
			return;
		}

		ImageDesc.Access = ERHIAccess::GPUOnly;
		SharedPtr<RHIImage> Image = Engine::Get().DynamicRHI().CreateImage(ImageDesc);
		if (Image == nullptr)
		{
			Import->State.set_value(EAssetImportResult::Failed);
			stbi_image_free(TextureData);
			return;
		}


		RenderThread::Enqueue([=](RHIContextImmediate& Context) 
			{
				void* Mapped = Context.Map(StagingImage, ERHIMapAccess::Write);
				if (Mapped == nullptr)
				{
					Import->State.set_value(EAssetImportResult::Failed);
					stbi_image_free(TextureData);
					return;
				}

				memcpy(Mapped, TextureData, StagingImage->ByteSize());

				Context.UnMap(StagingImage);

				Context.CopyImage(StagingImage, Image);

				stbi_image_free(TextureData);

				Import->Result->Assets.Emplace(SharedPtr<Asset>(new Texture(Image)));

				Import->State.set_value(EAssetImportResult::Success);
			});
	}
}
