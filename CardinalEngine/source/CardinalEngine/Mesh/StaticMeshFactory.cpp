#include "CardinalEngine/Mesh/StaticMeshFactory.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace CE
{
	SharedPtr<StaticMesh> StaticMeshFactory::Plane(const Vec2f& Extent /*= Vec2f(1.0f)*/, const Color& Color /*= Color::White*/)
	{
		const Vec4f ColorVec = Vec4f(Color.R, Color.G, Color.B, Color.A);
		Vec2f HalfExtent = Extent / 2;

		Array<MeshVertex> Vertices;
		Vertices.Reserve(4);
		Array<uint32> Indices;
		Indices.Reserve(6);

		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec2f(0.0f, 0.0f), ColorVec });
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec2f(1.0f, 0.0f), ColorVec });
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec2f(1.0f, 1.0f), ColorVec });
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec2f(0.0f, 1.0f), ColorVec });
		Indices.Add({ 0, 1, 2, 2, 3, 0 });

		return SharedPtr<StaticMesh>(new StaticMesh(Vertices, Indices));
	}

	SharedPtr<StaticMesh> StaticMeshFactory::Cube(const Vec3f& Extent /*= Vec3f(1.0f)*/, const Color& Color /*= Color::White*/)
	{
		Vec3f HalfExtent = -(Extent / 2);
		Vec4f ColorVec = Vec4f(Color.R, Color.G, Color.B, Color.A);

		Array<MeshVertex> Vertices;
		Vertices.Reserve(24);
		Array<uint32> Indices;
		Indices.Reserve(36);

		// Front
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 0.0f, -1.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Front Top Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 0.0f, -1.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Front Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 0.0f, -1.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Front Bottom Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 0.0f, -1.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Front Bottom Left
		Indices.Add({ 0, 1, 2, 2, 3, 0 });

		// Back
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 0.0f, 1.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Back Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 0.0f, 1.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Back Top Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 0.0f, 1.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Back Bottom Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 0.0f, 1.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Back Bottom Right
		Indices.Add({ 4, 5, 6, 6, 7, 4 });

		// Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(-1.0f, 0.0f, 0.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Back Top Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(-1.0f, 0.0f, 0.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Front Top Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(-1.0f, 0.0f, 0.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Front Bottom Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(-1.0f, 0.0f, 0.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Back Bottom Left
		Indices.Add({ 8, 9, 10, 10, 11, 8 });

		// Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(1.0f, 0.0f, 0.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Front Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(1.0f, 0.0f, 0.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Back Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(1.0f, 0.0f, 0.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Back Bottom Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(1.0f, 0.0f, 0.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Front Bottom Right
		Indices.Add({ 12, 13, 14, 14, 15, 12 });

		// Top
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 1.0f, 0.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Back Top Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, 1.0f, 0.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Back Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 1.0f, 0.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Front Top Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, 1.0f, 0.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Front Top Left
		Indices.Add({ 16, 17, 18, 18, 19, 16 });

		// Bottom
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, -1.0f, 0.0f), Vec2f(1.0f, 1.0f), ColorVec });	// Front Bottom Left
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z), -Vec3f(0.0f, -1.0f, 0.0f), Vec2f(0.0f, 1.0f), ColorVec });	// Front Bottom Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(+HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, -1.0f, 0.0f), Vec2f(0.0f, 0.0f), ColorVec });	// Back Bottom Right
		Vertices.EmplaceRef(MeshVertex{ Vec3f(-HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z), -Vec3f(0.0f, -1.0f, 0.0f), Vec2f(1.0f, 0.0f), ColorVec });	// Back Bottom Left
		Indices.Add({ 20, 21, 22, 22, 23, 20 });

		return SharedPtr<StaticMesh>(new StaticMesh(Vertices, Indices));
	}

	SharedPtr<StaticMesh> StaticMeshFactory::UVSphere(float Radius /*= 1.0f*/, uint32 LatitudeBands /*= 30*/, uint32 LongitudeBands /*= 30*/, const Color& Color /*= Color::White*/)
	{
		const Vec4f ColorVec = Vec4f(Color.R, Color.G, Color.B, Color.A);

		Array<MeshVertex> Vertices;
		Array<uint32> Indices;

		for (float latitude = 0.0f; latitude <= LatitudeBands; latitude++)
		{
			const float theta = latitude * (float)M_PI / LatitudeBands;
			const float sinTheta = std::sin(theta);
			const float cosTheta = std::cos(theta);

			for (float longitude = 0.0f; longitude <= LongitudeBands; longitude++)
			{
				const float phi = longitude * 2.f * (float)M_PI / LongitudeBands;
				const float sinPhi = std::sin(phi);
				const float cosPhi = std::cos(phi);

				MeshVertex& NewVertex = Vertices.EmplaceRef();
				NewVertex.Normal = Vec3f(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				NewVertex.Position = Vec3f(NewVertex.Normal * Radius);
				NewVertex.Color = ColorVec;
			}
		}

		for (uint32_t latitude = 0; latitude < (uint32_t)LatitudeBands; latitude++)
		{
			for (uint32_t longitude = 0; longitude < (uint32_t)LongitudeBands; longitude++)
			{
				const uint32_t first = (latitude * ((uint32_t)LongitudeBands + 1)) + longitude;
				const uint32_t second = first + (uint32_t)LongitudeBands + 1;

				Indices.Emplace(first);
				Indices.Emplace(second);
				Indices.Emplace(first + 1);

				Indices.Emplace(second);
				Indices.Emplace(second + 1);
				Indices.Emplace(first + 1);
			}
		}

		return SharedPtr<StaticMesh>(new StaticMesh(Vertices, Indices));
	}

	SharedPtr<StaticMesh> StaticMeshFactory::IcoSphere(float Radius, uint32 Subdivides /*= 3*/, const Color& Color /*= Color::White*/)
	{
		const Vec4f ColorVec = Vec4f(Color.R, Color.G, Color.B, Color.A);

		Array<MeshVertex> Vertices;
		Array<uint32> Indices;

		using Index = uint32;

		struct Triangle
		{
			Index vertex[3];
		};

		using TriangleList = std::vector<Triangle>;
		using VertexList = std::vector<Vec3f>;

		const float X = .525731112119133606f * Radius;
		const float Z = .850650808352039932f * Radius;
		const float N = 0.0f * Radius;

		const VertexList vertices =
		{
		  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
		  {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
		  {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
		};

		const TriangleList triangles =
		{
		  {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
		  {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
		  {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
		  {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
		};

		using Lookup = std::map<std::pair<Index, Index>, Index>;

		auto vertex_for_edge = [&](Lookup& lookup,
			VertexList& vertices, Index first, Index second) -> Index
		{
			Lookup::key_type key(first, second);
			if (key.first > key.second)
				std::swap(key.first, key.second);

			auto inserted = lookup.insert({ key, vertices.size() });
			if (inserted.second)
			{
				auto& edge0 = vertices[first];
				auto& edge1 = vertices[second];
				auto point = (edge0 + edge1).Normalize();
				vertices.push_back(point * Radius);
			}

			return inserted.first->second;
		};

		auto subdivide = [&](VertexList& vertices,
			TriangleList triangles) -> TriangleList
		{
			Lookup lookup;
			TriangleList result;

			for (auto&& each : triangles)
			{
				std::array<Index, 3> mid;
				for (int edge = 0; edge < 3; ++edge)
				{
					mid[edge] = vertex_for_edge(lookup, vertices,
						each.vertex[edge], each.vertex[(edge + 1) % 3]);
				}

				result.push_back({ each.vertex[0], mid[0], mid[2] });
				result.push_back({ each.vertex[1], mid[1], mid[0] });
				result.push_back({ each.vertex[2], mid[2], mid[1] });
				result.push_back({ mid[0], mid[1], mid[2] });
			}

			return result;
		};

		using IndexedMesh = std::pair<VertexList, TriangleList>;

		VertexList FinalVertices = vertices;
		TriangleList FinalTriangles = triangles;

		for (int i = 0; i < Subdivides; ++i)
		{
			FinalTriangles = subdivide(FinalVertices, FinalTriangles);
		}

		for (auto& Vert : FinalVertices)
		{
			MeshVertex& NewVertex = Vertices.EmplaceRef();
			NewVertex.Position = Vert;
			NewVertex.Color = ColorVec;
		}

		for (auto& Triangle : FinalTriangles)
		{
			Indices.Emplace(Triangle.vertex[0]);
			Indices.Emplace(Triangle.vertex[1]);
			Indices.Emplace(Triangle.vertex[2]);
		}

		return SharedPtr<StaticMesh>(new StaticMesh(Vertices, Indices));
	}

	SharedPtr<StaticMesh> StaticMeshFactory::QuadSphere(float Radius, uint32 Subdivides /*= 3*/, const Color& Color /*= Color::White*/)
	{
		const Vec4f ColorVec = Vec4f(Color.R, Color.G, Color.B, Color.A);

		Array<MeshVertex> Vertices;
		Array<uint32> Indices;


		return SharedPtr<StaticMesh>(new StaticMesh());
	}

	SharedPtr<StaticMesh> StaticMeshFactory::Cylinder(float Height /*= 1.0f*/, float Radius /*= 1.0f*/, uint32 LongitudeBands /*= 30*/, const Color& Color /*= Color::White*/)
	{
		return SharedPtr<StaticMesh>(new StaticMesh());
	}

	static void CalculateRing(size_t segments, float radius, float y, float dy, float height, float actualRadius, Array<MeshVertex>& Vertices)
	{
		float segIncr = 1.0f / (float)(segments - 1);
		for (size_t s = 0; s < segments; s++)
		{
			float x = std::cos(float(M_PI * 2) * s * segIncr) * radius;
			float z = std::sin(float(M_PI * 2) * s * segIncr) * radius;

			MeshVertex& Vertex = Vertices.EmplaceRef();
			Vertex.Position = Vec3f(actualRadius * x, actualRadius * y + height * dy, actualRadius * z);
		}
	}

	SharedPtr<StaticMesh> StaticMeshFactory::Capsule(float Height /*= 1.0f*/, float Radius /*= 1.0f*/, const Color& Color /*= Color::White*/)
	{
		constexpr size_t subdivisionsHeight = 8;
		constexpr size_t ringsBody = subdivisionsHeight + 1;
		constexpr size_t ringsTotal = subdivisionsHeight + ringsBody;
		constexpr size_t numSegments = 12;
		constexpr float radiusModifier = 0.021f; // Needed to ensure that the wireframe is always visible

		Array<MeshVertex> Vertices;
		Array<uint32> Indices;

		Vertices.Reserve(numSegments * ringsTotal);
		Indices.Reserve((numSegments - 1) * (ringsTotal - 1) * 2);

		float bodyIncr = 1.0f / (float)(ringsBody - 1);
		float ringIncr = 1.0f / (float)(subdivisionsHeight - 1);

		for (int r = 0; r < subdivisionsHeight / 2; r++)
			CalculateRing(numSegments, std::sin(float(M_PI) * r * ringIncr), std::sin(float(M_PI) * (r * ringIncr - 0.5f)), -0.5f, Height, Radius + radiusModifier, Vertices);

		for (int r = 0; r < ringsBody; r++)
			CalculateRing(numSegments, 1.0f, 0.0f, r * bodyIncr - 0.5f, Height, Radius + radiusModifier, Vertices);

		for (int r = subdivisionsHeight / 2; r < subdivisionsHeight; r++)
			CalculateRing(numSegments, std::sin(float(M_PI) * r * ringIncr), std::sin(float(M_PI) * (r * ringIncr - 0.5f)), 0.5f, Height, Radius + radiusModifier, Vertices);

		for (int r = 0; r < ringsTotal - 1; r++)
		{
			for (int s = 0; s < numSegments - 1; s++)
			{
				Indices.Emplace((uint32_t)(r * numSegments + s + 1));
				Indices.Emplace((uint32_t)(r * numSegments + s + 0));
				Indices.Emplace((uint32_t)((r + 1) * numSegments + s + 1));

				Indices.Emplace((uint32_t)((r + 1) * numSegments + s + 0));
				Indices.Emplace((uint32_t)((r + 1) * numSegments + s + 1));
				Indices.Emplace((uint32_t)(r * numSegments + s));
			}
		}

		return SharedPtr<StaticMesh>(new StaticMesh(Vertices, Indices));
	}

	SharedPtr<StaticMesh> StaticMeshFactory::Pyramid(float Height /*= 1.0f*/, float FloorRadius /*= 1.0f*/, uint32 Sides /*= 4*/, const Color& Color /*= Color::White*/)
	{
		return SharedPtr<StaticMesh>(new StaticMesh());
	}
}
