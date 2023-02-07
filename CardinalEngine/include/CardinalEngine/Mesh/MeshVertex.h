#pragma once
#include "../Framework/Core/Headers.h"
#include "../Rendering/RHI/Pipeline.h"

namespace CE
{
	const Array<ShaderVertexInput> MeshVertexLayout = {
			ShaderVertexInput(0, EDataFormat::RGB32_Float),
			ShaderVertexInput(1, EDataFormat::RGB32_Float),
			ShaderVertexInput(2, EDataFormat::RG32_Float),
			ShaderVertexInput(3, EDataFormat::RGBA32_Float),
			ShaderVertexInput(4, EDataFormat::RGB32_Float),
	};

	struct MeshVertex
	{
		Vec3f Position = Vec3f(0.0f);
		Vec3f Normal = Vec3f(0.0f);
		Vec2f UV = Vec2f(0.0f, 0.0f);
		Vec4f Color = Vec4f(1.0f);
		Vec3f Tangent = Vec3f(0.0f);
	};
}
