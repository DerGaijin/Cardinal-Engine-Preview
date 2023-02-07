#pragma once
#include "../../Framework/Core/Headers.h"
#include "RenderProxy.h"
#include "../../AssetSystem/AssetRef.h"

namespace CE
{
	class RHIContextImmediate;
	class Material;

	class CE_API RP_Geometry : public RenderProxy
	{
	public:
		virtual void DrawGeometry(RHIContextImmediate& Context) = 0;

		virtual const AssetRef<Material>& GetMaterial() = 0;
	};
}
