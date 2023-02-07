#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/Object/Object.h"

namespace CE
{
	class Archive;

	// Asset base class that can be serialized
	class CE_API Asset : public Object
	{
		CE_OBJECT_BODY();

		friend class AssetStorage;
	public:


	protected:
		virtual void SerializeAsset(Archive& Ar) {}

	};
}
