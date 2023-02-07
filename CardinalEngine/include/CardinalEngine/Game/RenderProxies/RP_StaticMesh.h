#pragma once
#include "../../Framework/Core/Headers.h"
#include "RP_Geometry.h"
#include "../../AssetSystem/AssetRef.h"
#include "../../Mesh/StaticMesh.h"

namespace CE
{
	class RHIBuffer;

	class CE_API RP_StaticMesh : public RP_Geometry
	{
	public:
		RP_StaticMesh();

		virtual void DrawGeometry(RHIContextImmediate& Context) override;

		virtual const AssetRef<Material>& GetMaterial() override;

		void SetStaticMesh(const AssetRef<StaticMesh>& StaticMesh);
		
		void SetMaterial(const AssetRef<Material>& Material);

		void SetTransform(RHIContextImmediate& Context, const Mat4x4f& Transform);

	private:
		SharedPtr<RHIBuffer> m_Transform;
		AssetRef<StaticMesh> m_StaticMesh;
		AssetRef<Material> m_Material;
	};
}
