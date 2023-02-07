#pragma once
#include "../../../CardinalEngine/Framework/Core/Headers.h"
#include "../../../CardinalEngine/Rendering/RenderTarget.h"
#include "../EditorWindow.h"
#include "../EditorViewport.h"

namespace CE
{
	class Entity;
	class EntityComponent;
	class TransformComponent;
	class CameraComponent;
	class EditorViewportEntity;

	class TMP_Image_RenderTarget : public RenderTarget
	{
	public:
		TMP_Image_RenderTarget(const SharedPtr<RHIImage>& RT) : m_TargetImage(RT) {}

		virtual SharedPtr<RHIImage> GetTargetImage() const override
		{
			return m_TargetImage;
		}

	private:
		SharedPtr<RHIImage> m_TargetImage;
	};


	class CE_API MainEditor : public EditorWindow
	{
	public:
		virtual void Tick(float DeltaTime) override;

		virtual void Draw(float DeltaTime) override;

	private:
		void DrawViewport(ImGuiWindowClass* WindowClass);
		void DrawWorldHierarchy(ImGuiWindowClass* WindowClass);
		void DrawWorldSettings(ImGuiWindowClass* WindowClass);
		void DrawEntityInspector(ImGuiWindowClass* WindowClass);
		void DrawEntityInspector_CompSelect(const SharedPtr<Entity>& SelectedEntity);
		void DrawEntityInspector_CompHierarchy(const SharedPtr<Entity>& SelectedEntity, const SharedPtr<TransformComponent>& Component, const SharedPtr<EntityComponent>& SelectedComponent);

	private:
		EditorViewport Viewport;
	};
}
