#include "CardinalEngine-Editor/Game/WorldDebugDraw.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"

namespace CE
{
	void WorldDebugDraw::Line(const Vec3f& Start, const Vec3f& End, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{
		LineBatch({ Line3D{ Start, End} }, Color, DurationInSeconds);
	}

	void WorldDebugDraw::Plane(const Vec3f& Location, const Quaternion& Rotation, const Vec2f& Extent, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{
		Vec2f HalfExtent = Extent / 2.0f;

		Vec3f XNYN = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, -HalfExtent.Y, 0.0f));
		Vec3f XNYP = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, +HalfExtent.Y, 0.0f));
		Vec3f XPYP = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, +HalfExtent.Y, 0.0f));
		Vec3f XPYN = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, -HalfExtent.Y, 0.0f));

		Array<Line3D> Lines;
		Lines.Reserve(12);

		Lines.Emplace(Line3D{ XNYN, XNYP });
		Lines.Emplace(Line3D{ XNYP, XPYP });
		Lines.Emplace(Line3D{ XPYP, XPYN });
		Lines.Emplace(Line3D{ XPYN, XNYN });

		LineBatch(Lines, Color, DurationInSeconds);
	}

	void WorldDebugDraw::Grid2D(const Vec3f& Location, const Quaternion& Rotation, const Vec2f& Extent, const Vec2ui& Count, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{
		if (Count.X == 0 || Count.Y == 0)
		{
			return;
		}

		Vec2f HalfSize = (Extent * Count) / 2.0f;

		Array<Line3D> Lines;
		Lines.Reserve(((Count.X + 1) * 2) + ((Count.Y + 1) * 2));

		auto GetLocation = [&](uint32 X, uint32 Y)
		{
			return Location + Rotation.Rotate(Vec3f((X * Extent.X) - HalfSize.X, (Y * Extent.Y) - HalfSize.Y, 0.0f));
		};

		for (size_t Y = 0; Y < Count.Y + 1; Y++)
		{
			for (size_t X = 0; X < Count.X + 1; X++)
			{
				if (X != Count.X)
				{
					Lines.Emplace(Line3D{ GetLocation(X, Y), GetLocation(X + 1, Y) });
				}
				if (Y != Count.Y)
				{
					Lines.Emplace(Line3D{ GetLocation(X, Y), GetLocation(X, Y + 1) });
				}
			}
		}

		LineBatch(Lines, Color, DurationInSeconds);
	}

	void WorldDebugDraw::Cube(const Vec3f& Location, const Quaternion& Rotation, const Vec3f& Extent /*= 1.0f*/, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{
		Vec3f HalfExtent = Extent / 2.0f;

		Vec3f TopFrontLeftCorner = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z));
		Vec3f TopBackLeftCorner = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z));
		Vec3f TopBackRightCorner = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, +HalfExtent.Y, +HalfExtent.Z));
		Vec3f TopFrontRightCorner = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, +HalfExtent.Y, -HalfExtent.Z));

		Vec3f BottomFrontLeftCorner = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z));
		Vec3f BottomBackLeftCorner = Location + Rotation.Rotate(Vec3f(-HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z));
		Vec3f BottomFrontRightCorner = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z));
		Vec3f BottomBackRightCorner = Location + Rotation.Rotate(Vec3f(+HalfExtent.X, -HalfExtent.Y, +HalfExtent.Z));

		Array<Line3D> Lines;
		Lines.Reserve(12);

		Lines.Emplace(Line3D{ TopFrontLeftCorner, TopBackLeftCorner }); // Top Left Edge
		Lines.Emplace(Line3D{ TopBackLeftCorner, TopBackRightCorner }); // Top Back Edge
		Lines.Emplace(Line3D{ TopBackRightCorner, TopFrontRightCorner }); // Top Right Edge
		Lines.Emplace(Line3D{ TopFrontRightCorner, TopFrontLeftCorner }); // Top Front Edge
		Lines.Emplace(Line3D{ BottomFrontLeftCorner, BottomBackLeftCorner }); // Bottom Left Edge
		Lines.Emplace(Line3D{ BottomBackLeftCorner, BottomBackRightCorner }); // Bottom Back Edge
		Lines.Emplace(Line3D{ BottomBackRightCorner, BottomFrontRightCorner }); // Bottom Right Edge
		Lines.Emplace(Line3D{ BottomFrontRightCorner, BottomFrontLeftCorner }); // Bottom Front Edge
		Lines.Emplace(Line3D{ BottomBackLeftCorner, TopBackLeftCorner }); // Back Left Edge
		Lines.Emplace(Line3D{ BottomBackRightCorner, TopBackRightCorner }); // Back Right Edge
		Lines.Emplace(Line3D{ BottomFrontLeftCorner, TopFrontLeftCorner }); // Front Left Edge
		Lines.Emplace(Line3D{ BottomFrontRightCorner, TopFrontRightCorner }); // Front Right Edge

		LineBatch(Lines, Color, DurationInSeconds);
	}

	void WorldDebugDraw::Sphere(const Vec3f& Location, float Radius /*= 1.0f*/, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{

	}

	void WorldDebugDraw::Cylinder(const Vec3f& Location, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{

	}

	void WorldDebugDraw::Capsule(const Vec3f& Location, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{

	}

	void WorldDebugDraw::Pyramid(const Vec3f& Location, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{

	}

	void WorldDebugDraw::Frustum(const Vec3f& Location, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{

	}

	void WorldDebugDraw::LineBatch(const Array<Line3D>& Lines, const Color& Color /*= Color::White*/, float DurationInSeconds /*= 0.0f*/)
	{
		std::chrono::high_resolution_clock::time_point TimePoint = std::chrono::high_resolution_clock::now();
		TimePoint += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<float>(DurationInSeconds));

		Array<LineInstance> LineInstances;
		LineInstances.Reserve(Lines.Size());

		for (auto& Line : Lines)
		{
			Mat4x4f& LineTransform = LineInstances.EmplaceRef(LineInstance{ Mat4x4f::Identity(), Color }).Transform;

			LineTransform = LineTransform.Translate(Line.Start);
			LineTransform *= Mat4x4f::FromQuaternion(Quaternion::LookAtSafe(Line.Start, Line.End, World::UpVector));
			LineTransform = LineTransform.Scale(Vec3f(1.0f, Line.Start.Distance(Line.End), 1.0f));
		}

		size_t Index = 0;
		for (auto& LineTime : m_LineTimes)
		{
			if (LineTime >= TimePoint)
			{
				break;
			}
			Index++;
		}

		m_LineTimes.Insert(Index, TimePoint, LineInstances.Size());
		m_Lines.Insert(Index, LineInstances);
		m_LinesChanged = true;
	}

	void WorldDebugDraw::RemoveOutdatedLines()
	{
		std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();

		size_t Count = 0;
		for (auto& Line : m_LineTimes)
		{
			if (Line >= Now)
			{
				break;
			}
			Count++;
		}

		if (Count > 0)
		{
			m_LineTimes.RemoveAt(0, Count);
			m_Lines.RemoveAt(0, Count);
			m_LinesChanged = true;
		}
	}

	void WorldDebugDraw::UpdateLineBuffer()
	{
		RHIBufferDescription TransformBufferDesc;
		TransformBufferDesc.Size = sizeof(LineInstance) * m_Lines.Size();
		TransformBufferDesc.ElementSize = sizeof(LineInstance);
		TransformBufferDesc.Access = ERHIAccess::GPURead_CPUWrite;
		TransformBufferDesc.Usage = ERHIBufferUsage::VertexBuffer;
		m_LineBuffer = Engine::Get().DynamicRHI().CreateBuffer(TransformBufferDesc);

		SharedPtr<RHIBuffer> LineBuffer = m_LineBuffer;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				void* Data = Context.Map(LineBuffer, ERHIMapAccess::Write);
		if (Data)
		{
			memcpy(Data, m_Lines.Data(), TransformBufferDesc.Size);
			Context.UnMap(LineBuffer);
		}
			});
	}
}
