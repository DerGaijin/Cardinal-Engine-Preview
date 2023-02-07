#include "CardinalEngine/Mesh/StaticMesh.h"
#include "CardinalEngine/Framework/Serialization/Archive.h"

#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"

namespace CE
{
	StaticMesh::StaticMesh(const Array<MeshVertex>& Vertices, const Array<uint32>& Indices)
	{
		RHIBufferDescription VertexBufferDesc;
		VertexBufferDesc.Size = Vertices.Size() * sizeof(MeshVertex);
		VertexBufferDesc.ElementSize = sizeof(MeshVertex);
		VertexBufferDesc.Usage = ERHIBufferUsage::VertexBuffer;
		VertexBufferDesc.Access = ERHIAccess::All;
		SharedPtr<RHIBuffer> VertexStagingBuffer = Engine::Get().DynamicRHI().CreateBuffer(VertexBufferDesc);
		
		VertexBufferDesc.Access = ERHIAccess::GPUOnly;
		m_VertexBuffer = Engine::Get().DynamicRHI().CreateBuffer(VertexBufferDesc);


		RHIBufferDescription IndexBufferDesc;
		IndexBufferDesc.Size = Indices.Size() * sizeof(uint32);
		IndexBufferDesc.ElementSize = sizeof(uint32);
		IndexBufferDesc.Usage = ERHIBufferUsage::IndexBuffer;
		IndexBufferDesc.Access = ERHIAccess::All;
		SharedPtr<RHIBuffer> IndexStagingBuffer = Engine::Get().DynamicRHI().CreateBuffer(IndexBufferDesc);
		
		IndexBufferDesc.Access = ERHIAccess::GPUOnly;
		m_IndexBuffer = Engine::Get().DynamicRHI().CreateBuffer(IndexBufferDesc);


		Array<MeshVertex> VerticesCopy = Vertices;
		Array<uint32> IndicesCopy = Indices;
		SharedPtr<RHIBuffer> VertexBuffer = m_VertexBuffer;
		SharedPtr<RHIBuffer> IndexBuffer = m_IndexBuffer;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				void* Mapped = Context.Map(VertexStagingBuffer, ERHIMapAccess::Write);
				if (Mapped == nullptr)
				{
					return;
				}
				memcpy(Mapped, VerticesCopy.Data(), VerticesCopy.Size() * sizeof(MeshVertex));
				Context.UnMap(VertexStagingBuffer);

				Mapped = Context.Map(IndexStagingBuffer, ERHIMapAccess::Write);
				if (Mapped == nullptr)
				{
					return;
				}
				memcpy(Mapped, IndicesCopy.Data(), IndicesCopy.Size() * sizeof(uint32));
				Context.UnMap(IndexStagingBuffer);

				Context.CopyBuffer(VertexStagingBuffer, VertexBuffer);
				Context.CopyBuffer(IndexStagingBuffer, IndexBuffer);
			});
	}

	StaticMesh::StaticMesh(const SharedPtr<RHIBuffer>& VertexBuffer, const SharedPtr<RHIBuffer>& IndexBuffer)
	{
		CE_CHECK(VertexBuffer != nullptr && VertexBuffer->Description.Usage == ERHIBufferUsage::VertexBuffer && VertexBuffer->Description.ElementSize == sizeof(MeshVertex));
		CE_CHECK(IndexBuffer != nullptr && IndexBuffer->Description.Usage == ERHIBufferUsage::IndexBuffer && IndexBuffer->Description.ElementSize == sizeof(uint32));

		m_VertexBuffer = VertexBuffer;
		m_IndexBuffer = IndexBuffer;
	}

	void StaticMesh::SerializeAsset(Archive& Ar)
	{

	}
}
