#pragma once
#include "../../Framework/Core/Headers.h"
#include "GeometryComponent.h"
#include "../../AssetSystem/AssetRef.h"
#include "../../Mesh/StaticMesh.h"
#include "../../Material/Material.h"

namespace CE
{
	class RP_StaticMesh;

	class CE_API StaticMeshComponent : public GeometryComponent
	{
		CE_OBJECT_BODY();
	public:
		StaticMeshComponent();

		void SetStaticMesh(const AssetRef<StaticMesh>& StaticMesh);

		void SetMaterial(const AssetRef<Material>& Material);

		const AssetRef<StaticMesh>& GetStaticMesh() const
		{
			return m_StaticMesh;
		}

		const AssetRef<Material>& GetMaterial() const
		{
			return m_Material;
		}

	protected:
		virtual void UpdateWorldTransform() override;

		virtual SharedPtr<RenderProxy> GetRenderProxy() const override;

	private:
		SharedPtr<RP_StaticMesh> m_RenderProxy;

		AssetRef<StaticMesh> m_StaticMesh;
		AssetRef<Material> m_Material;
	};
}
