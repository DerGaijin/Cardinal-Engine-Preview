#include "CardinalEngine/Game/RenderProxies/RP_StaticMesh.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"

namespace CE
{
	RP_StaticMesh::RP_StaticMesh()
	{
		RHIBufferDescription Desc;
		Desc.Size = sizeof(Mat4x4f);
		Desc.ElementSize = sizeof(Mat4x4f);
		Desc.Usage = ERHIBufferUsage::VertexBuffer;
		Desc.Access = ERHIAccess::GPURead_CPUWrite;	// Maybe Use Staging
		m_Transform = Engine::Get().DynamicRHI().CreateBuffer(Desc);
	}

	void RP_StaticMesh::DrawGeometry(RHIContextImmediate& Context)
	{
		SharedPtr<StaticMesh> StaticMesh = m_StaticMesh.Get();
		if (StaticMesh != nullptr)
		{
			Array<SharedPtr<RHIBuffer>> VertexBuffers = { StaticMesh->VertexBuffer(), m_Transform };
			Array<uint32> Offsets;
			Offsets.AddZeroed(VertexBuffers.Size());
			Context.BindVertexBuffers(VertexBuffers.Size(), VertexBuffers.Data(), Offsets.Data());

			SharedPtr<RHIBuffer> IndexBuffer = StaticMesh->IndexBuffer();
			Context.BindIndexBuffer(IndexBuffer);

			Context.DrawIndexedPrimitive(IndexBuffer->Description.Size / IndexBuffer->Description.ElementSize);
		}
	}

	const AssetRef<Material>& RP_StaticMesh::GetMaterial()
	{
		return m_Material;
	}

	void RP_StaticMesh::SetStaticMesh(const AssetRef<StaticMesh>& StaticMesh)
	{
		m_StaticMesh = StaticMesh;
	}

	void RP_StaticMesh::SetMaterial(const AssetRef<Material>& Material)
	{
		m_Material = Material;
	}

	void RP_StaticMesh::SetTransform(RHIContextImmediate& Context, const Mat4x4f& Transform)
	{
		void* Mapped = Context.Map(m_Transform, ERHIMapAccess::Write);
		if (Mapped == nullptr)
		{
			return;
		}
		memcpy(Mapped, &Transform, sizeof(Transform));
		Context.UnMap(m_Transform);
	}
}
