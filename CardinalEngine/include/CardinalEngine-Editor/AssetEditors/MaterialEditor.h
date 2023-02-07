#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "AssetEditor.h"
#include "../UI/EditorViewport.h"
#include "../../CardinalEngine/Material/Material.h"

namespace CE
{
	class World;
	class Entity;
	class StaticMeshComponent;

	class CE_API MaterialEditor : public AssetEditor
	{
	public:
		MaterialEditor(size_t StorageHash, const std::filesystem::path& Path);

		virtual void Tick(float DeltaTime) override;

		virtual void Draw(float DeltaTime) override;
	

	private:
		void Draw_Viewport();
		void Draw_ShaderEditor();
		void Draw_Properties();
		void Draw_Settings();
		void Draw_Errors();

		void UpdatePreviewMesh();
		void RebuildMaterial();

	private:
		SharedPtr<Material> m_Material;

		EditorViewport m_Viewport;
		int PreviewMesh = 1;

		MaterialSettings m_MatSettings;
		bool m_RebuildMaterial = true;
		Array<String> m_CompileErrors;

		SharedPtr<World> m_PreviewWorld;
		SharedPtr<Entity> m_PreviewEntity;
		SharedPtr<StaticMeshComponent> m_PreviewMeshComponent;
	};
}
