#include "CardinalEngine/Game/Components/StaticMeshComponent.h"
#include "CardinalEngine/Game/RenderProxies/RP_StaticMesh.h"
#include "CardinalEngine/Engine/Engine.h"

namespace CE
{
	StaticMeshComponent::StaticMeshComponent()
	{
		m_RenderProxy = SharedPtr<RP_StaticMesh>(new RP_StaticMesh());

		UpdateWorldTransform();
	}

	void StaticMeshComponent::SetStaticMesh(const AssetRef<StaticMesh>& StaticMesh)
	{
		SharedPtr<RP_StaticMesh> StaticMeshProxy = m_RenderProxy;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				StaticMeshProxy->SetStaticMesh(StaticMesh);
			});
	}

	void StaticMeshComponent::SetMaterial(const AssetRef<Material>& Material)
	{
		SharedPtr<RP_StaticMesh> StaticMeshProxy = m_RenderProxy;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				StaticMeshProxy->SetMaterial(Material);
			});
	}

	void StaticMeshComponent::UpdateWorldTransform()
	{
		TransformComponent::UpdateWorldTransform();

		Mat4x4f Transform = GetWorldTransform().ToMatrix();
		SharedPtr<RP_StaticMesh> StaticMeshProxy = m_RenderProxy;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				StaticMeshProxy->SetTransform(Context, Transform);
			});
	}

	SharedPtr<RenderProxy> StaticMeshComponent::GetRenderProxy() const
	{
		return m_RenderProxy;
	}
}
