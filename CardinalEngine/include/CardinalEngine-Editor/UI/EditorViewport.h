#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../../CardinalEngine/Rendering/RenderTarget.h"

namespace CE
{
	class World;
	class EditorViewportEntity;

	//@TODO: This is temporary till we got a real one
	class ImageRenderTarget : public RenderTarget
	{
	public:
		ImageRenderTarget(const SharedPtr<RHIImage>& RT) : m_TargetImage(RT) {}

		virtual SharedPtr<RHIImage> GetTargetImage() const override
		{
			return m_TargetImage;
		}

	private:
		SharedPtr<RHIImage> m_TargetImage;
	};

	class CE_API EditorViewport : public NonCopyable
	{
	public:
		EditorViewport();

		void Tick(float DeltaTime);

		void ImGuiDraw(const Vec2f& Size);
	
		void SetWorld(const SharedPtr<World>& World);

		SharedPtr<World> GetWorld() const
		{
			return m_World.Lock();
		}

		const SharedPtr<EditorViewportEntity>& Entity() const
		{
			return m_Entity;
		}

	private:
		WeakPtr<World> m_World;

		SharedPtr<EditorViewportEntity> m_Entity;

		uint32 m_RenderTargetWidth = 0;
		uint32 m_RenderTargetHeight = 0;
		SharedPtr<ImageRenderTarget> m_RenderTarget;

		bool m_ViewportInput = false;
	};
}
