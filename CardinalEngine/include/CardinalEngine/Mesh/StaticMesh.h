#pragma once
#include "../Framework/Core/Headers.h"
#include "../AssetSystem/Asset.h"
#include "MeshVertex.h"

namespace CE
{
	class RHIBuffer;

	class CE_API StaticMesh : public Asset
	{
		CE_OBJECT_BODY()
	public:
		StaticMesh() = default;
		StaticMesh(const Array<MeshVertex>& Vertices, const Array<uint32>& Indices);
		StaticMesh(const SharedPtr<RHIBuffer>& VertexBuffer, const SharedPtr<RHIBuffer>& IndexBuffer);

	protected:
		virtual void SerializeAsset(Archive& Ar) override;

	public:
		SharedPtr<RHIBuffer> VertexBuffer() const
		{
			return m_VertexBuffer;
		}

		SharedPtr<RHIBuffer> IndexBuffer() const
		{
			return m_IndexBuffer;
		}

	private:
		SharedPtr<RHIBuffer> m_VertexBuffer;
		SharedPtr<RHIBuffer> m_IndexBuffer;
	};
}
