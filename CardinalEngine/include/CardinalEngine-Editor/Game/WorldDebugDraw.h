#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"

namespace CE
{
	class RHIPipeline_Graphic;
	class RHIBuffer;

	class CE_API WorldDebugDraw : public NonCopyable
	{
	public:
		
		void Line(const Vec3f& Start, const Vec3f& End, const Color& Color = Color::White, float DurationInSeconds = 0.0f);
	
		void Plane(const Vec3f& Location, const Quaternion& Rotation, const Vec2f& Extent, const Color& Color = Color::White, float DurationInSeconds = 0.0f);
	
		void Grid2D(const Vec3f& Location, const Quaternion& Rotation, const Vec2f& Extent, const Vec2ui& Count, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Cube(const Vec3f& Location, const Quaternion& Rotation, const Vec3f& Extent = 1.0f, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Sphere(const Vec3f& Location, float Radius = 1.0f, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Cylinder(const Vec3f& Location, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Capsule(const Vec3f& Location, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Pyramid(const Vec3f& Location, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void Frustum(const Vec3f& Location, const Color& Color = Color::White, float DurationInSeconds = 0.0f);


		void LineBatch(const Array<Line3D>& Lines, const Color& Color = Color::White, float DurationInSeconds = 0.0f);

		void RemoveOutdatedLines();

		bool RequiresLineBufferUpdate() const
		{
			return m_LinesChanged;
		}

		void UpdateLineBuffer();

	public:
		const SharedPtr<RHIBuffer>& LineBuffer() const
		{
			return m_LineBuffer;
		}


	public:
		SharedPtr<RHIPipeline_Graphic> s_Pipeline;
		SharedPtr<RHIBuffer> s_VertexBuffer;

	private:
		Array<std::chrono::steady_clock::time_point> m_LineTimes;
		struct LineInstance 
		{
			Mat4x4f Transform;
			Color Color;
		};
		Array<LineInstance> m_Lines;
		SharedPtr<RHIBuffer> m_LineBuffer;

		bool m_LinesChanged = false;
	};
}
