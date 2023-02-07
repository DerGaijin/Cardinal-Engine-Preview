#pragma once
#include "../Framework/Core/Headers.h"
#include "../AssetSystem/Asset.h"

namespace CE
{
	class RHIImage;

	class CE_API Texture : public Asset
	{
		CE_OBJECT_BODY()
	public:
		Texture() = default;

		Texture(const SharedPtr<RHIImage>& Image) : m_Image(Image) {}

	public:
		const SharedPtr<RHIImage>& Image() const
		{
			return m_Image;
		}

	protected:
		virtual void SerializeAsset(Archive& Ar) override;

	private:
		SharedPtr<RHIImage> m_Image;
	};
}
