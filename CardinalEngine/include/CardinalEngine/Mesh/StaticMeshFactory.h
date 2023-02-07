#pragma once
#include "../Framework/Core/Headers.h"
#include "StaticMesh.h"

namespace CE
{
	class CE_API StaticMeshFactory
	{
	public:
		StaticMeshFactory() = delete;

		static SharedPtr<StaticMesh> Plane(const Vec2f& Extent = Vec2f(1.0f), const Color& Color = Color::White);

		static SharedPtr<StaticMesh> Cube(const Vec3f& Extent = Vec3f(1.0f), const Color& Color = Color::White);

		static SharedPtr<StaticMesh> UVSphere(float Radius = 1.0f, uint32 LatitudeBands = 30, uint32 LongitudeBands = 30, const Color& Color = Color::White);

		static SharedPtr<StaticMesh> IcoSphere(float Radius, uint32 Subdivides = 3, const Color& Color = Color::White);

		static SharedPtr<StaticMesh> QuadSphere(float Radius, uint32 Subdivides = 3, const Color& Color = Color::White);

		static SharedPtr<StaticMesh> Cylinder(float Height = 1.0f, float Radius = 0.5f, uint32 LongitudeBands = 30, const Color& Color = Color::White);

		static SharedPtr<StaticMesh> Capsule(float Height = 1.0f, float Radius = 0.5f, const Color& Color = Color::White);

		static SharedPtr<StaticMesh> Pyramid(float Height = 1.0f, float FloorRadius = 1.0f, uint32 Sides = 4, const Color& Color = Color::White);

	};
}
