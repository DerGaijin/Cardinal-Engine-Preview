#include "CardinalEngine-Editor/AssetEditors/MaterialEditor.h"
#include "CardinalEngine-Editor/Game/EditorViewportEntity.h"
#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"

#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/Components/StaticMeshComponent.h"
#include "CardinalEngine/Mesh/StaticMeshFactory.h"

namespace CE
{
	MaterialEditor::MaterialEditor(size_t StorageHash, const std::filesystem::path& Path) : AssetEditor(StorageHash, Path)
	{
		m_Viewport.Entity()->EditorName = "Material Preview Viewport";

		m_Viewport.Entity()->GetRootComponent()->SetRelativeLocation(Vec3f(0.0f, -2.0f, 0.0f));

		AssetRef<Material> LoadedMat = Engine::Get().AssetSystem.Get<Material>(AssetID(StorageHash, Path));
		LoadedMat.Wait();
		if (LoadedMat.State() == EAssetState::Ready)
		{
			m_Material = LoadedMat.Get();
			m_MatSettings = m_Material->Settings();
		}
	}

	void MaterialEditor::Tick(float DeltaTime)
	{
		if (m_PreviewWorld == nullptr)
		{
			m_PreviewWorld = SharedPtr<World>(new World());
			m_PreviewWorld->EditorName = String::Format("Material Preview (%s)", GetSuffix().Data());

			EditorGameInstance::Get()->AddWorld(m_PreviewWorld);
		}

		if (m_Viewport.GetWorld() != m_PreviewWorld)
		{
			m_Viewport.SetWorld(m_PreviewWorld);
		}

		if (m_PreviewEntity == nullptr)
		{
			m_PreviewEntity = SharedPtr<Entity>(new Entity());
			m_PreviewEntity->EditorName = "Preview Entity";
		}

		if (m_PreviewEntity->World() != m_PreviewWorld)
		{
			m_PreviewWorld->AddEntity(m_PreviewEntity);
		}

		if (m_PreviewMeshComponent == nullptr)
		{
			m_PreviewMeshComponent = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
			m_PreviewMeshComponent->EditorName = "Preview Mesh";

			UpdatePreviewMesh();
		}

		if (m_PreviewMeshComponent->Entity() != m_PreviewEntity)
		{
			m_PreviewEntity->SetRootComponent(m_PreviewMeshComponent);
		}

		if (m_RebuildMaterial)
		{
			RebuildMaterial();
		}

		m_Viewport.Tick(DeltaTime);
	}

	void MaterialEditor::Draw(float DeltaTime)
	{
		String Name = String::Format("%s##MaterialEditor_%s", Path().filename().replace_extension("").u8string().c_str(), GetSuffix().Data());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(Name.Data(), &IsOpen);
		ImGui::PopStyleVar();

		ImGuiWindowClass MaterialEditorWndClass;
		MaterialEditorWndClass.ClassId = ImGui::GetID(Name.Data());
		MaterialEditorWndClass.DockingAllowUnclassed = false;

		bool SetDocking = ImGui::DockBuilderGetNode(MaterialEditorWndClass.ClassId) == nullptr;

		ImGui::DockSpace(MaterialEditorWndClass.ClassId, ImGui::GetContentRegionAvail(), 0, &MaterialEditorWndClass);

		ImGui::End();

		if (SetDocking)
		{
			ImGuiID Dock_R = 0;
			ImGuiID Dock_L = ImGui::DockBuilderSplitNode(MaterialEditorWndClass.ClassId, ImGuiDir_Left, 0.2f, nullptr, &Dock_R);

			ImGuiID Dock_LD = 0;
			ImGuiID Dock_LU = ImGui::DockBuilderSplitNode(Dock_L, ImGuiDir_Up, 0.5f, nullptr, &Dock_LD);

			ImGuiID Dock_RD = 0;
			ImGuiID Dock_RU = ImGui::DockBuilderSplitNode(Dock_R, ImGuiDir_Up, 0.8f, nullptr, &Dock_RD);

			ImGui::DockBuilderGetNode(Dock_LU)->WantHiddenTabBarToggle = true;
			ImGui::DockBuilderGetNode(Dock_RU)->WantHiddenTabBarToggle = true;
			ImGui::DockBuilderGetNode(Dock_RD)->WantHiddenTabBarToggle = true;

			ImGui::DockBuilderFinish(MaterialEditorWndClass.ClassId);

			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			ImGui::SetNextWindowDockID(Dock_LU);
			Draw_Viewport();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			ImGui::SetNextWindowDockID(Dock_RU);
			Draw_ShaderEditor();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			ImGui::SetNextWindowDockID(Dock_LD);
			Draw_Properties();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			ImGui::SetNextWindowDockID(Dock_LD);
			Draw_Settings();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			ImGui::SetNextWindowDockID(Dock_RD);
			Draw_Errors();
		}
		else
		{
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			Draw_Viewport();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			Draw_ShaderEditor();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			Draw_Properties();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			Draw_Settings();
			ImGui::SetNextWindowClass(&MaterialEditorWndClass);
			Draw_Errors();
		}
	}

	void MaterialEditor::Draw_Viewport()
	{
		String Name = String::Format("Viewport##MaterialEditor_Viewport_%s", GetSuffix().Data());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(Name.Data());
		ImGui::PopStyleVar();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Combo("##InputPreviewMesh", &PreviewMesh, "Plane\0Cube\0UVSphere\0IcoSphere\0QuadSphere\0Cylinder\0Capsule\0Pyramid\0Custom"))
		{
			UpdatePreviewMesh();
		}

		ImVec2 Avail = ImGui::GetContentRegionAvail();
		m_Viewport.ImGuiDraw({ Avail.x, Avail.y });

		ImGui::End();
	}

	void MaterialEditor::Draw_ShaderEditor()
	{
		String Name = String::Format("Shader##MaterialEditor_Shader_%s", GetSuffix().Data());
		ImGui::Begin(Name.Data());

		if (ImGui::Button("Save"))
		{
			Engine::Get().AssetSystem.Store(m_Material, AssetID(StorageHash(), Path()));
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - 100));
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Combo("##InputShaderType", (int*)&m_MatSettings.ShaderLanguage, "HLSL\0GLSL"))
		{
			m_RebuildMaterial = true;
		}
		if (ImGui::InputTextMultiline("##InputShader", &m_MatSettings.ShaderSource, ImGui::GetContentRegionAvail()))
		{
			m_RebuildMaterial = true;
		}

		ImGui::End();
	}

	void MaterialEditor::Draw_Properties()
	{
		String Name = String::Format("Properties##MaterialEditor_Props_%s", GetSuffix().Data());
		ImGui::Begin(Name.Data());
		ImGui::Columns(2);

		size_t ValueOffset = 0;
		for (auto& Parameter : m_Material->DynamicParameters())
		{
			ImGui::Text(Parameter.Name.Data());
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			switch (Parameter.Type)
			{
			default:
			case CE::EShaderVariableType::Unknown:
			{
				ImGui::Text("Unknown");
				break;
			}
			case CE::EShaderVariableType::Struct:
			{
				ImGui::Text("Struct");
				break;
			}
			case CE::EShaderVariableType::Bool:
			{
				bool Value = *((bool*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::Checkbox(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Float:
			{
				float Value = *((float*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragFloat(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value, 0.01f))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Float2:
			{
				Vec2f Value = *((Vec2f*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragFloat2(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X, 0.01f))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Float3:
			{
				Vec3f Value = *((Vec3f*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragFloat3(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X, 0.01f))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Float4:
			{
				Vec4f Value = *((Vec4f*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragFloat4(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X, 0.01f))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Float4x4:
			{
				ImGui::Text("Matrix");
				break;
			}
			case CE::EShaderVariableType::Int:
			{
				int32 Value = *((int32*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragInt(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Int2:
			{
				Vec2i Value = *((Vec2i*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragInt2(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Int3:
			{
				Vec3i Value = *((Vec3i*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragInt3(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::Int4:
			{
				Vec4i Value = *((Vec4i*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragInt4(String::Format("##Input_%s", Parameter.Name.Data()).Data(), &Value.X))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::UInt:
			{
				uint32 Value = *((uint32*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragScalarN(String::Format("##Input_%s", Parameter.Name.Data()).Data(), ImGuiDataType_U32, &Value, 1))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::UInt2:
			{
				Vec2ui Value = *((Vec2ui*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragScalarN(String::Format("##Input_%s", Parameter.Name.Data()).Data(), ImGuiDataType_U32, &Value.X, 2))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::UInt3:
			{
				Vec3ui Value = *((Vec3ui*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragScalarN(String::Format("##Input_%s", Parameter.Name.Data()).Data(), ImGuiDataType_U32, &Value.X, 3))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			case CE::EShaderVariableType::UInt4:
			{
				Vec4ui Value = *((Vec4ui*)(m_Material->DynamicParameterValues().Data() + ValueOffset));
				if (ImGui::DragScalarN(String::Format("##Input_%s", Parameter.Name.Data()).Data(), ImGuiDataType_U32, &Value.X, 4))
				{
					m_Material->SetParameter(Parameter.Name, Value);
				}
				break;
			}
			}

			ImGui::NextColumn();

			ValueOffset += Parameter.Size;
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void MaterialEditor::Draw_Settings()
	{
		String Name = String::Format("Settings##MaterialEditor_Settings_%s", GetSuffix().Data());
		ImGui::Begin(Name.Data());
		ImGui::Columns(2);

		ImGui::Text("Domain");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Combo("##Input_Domain", (int*)&m_MatSettings.Domain, "Geometry"))
		{
			m_RebuildMaterial = true;
		}

		ImGui::NextColumn();
		ImGui::Text("Blend");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Combo("##Input_Blend", (int*)&m_MatSettings.Blend, "Opaque\0Translucent"))
		{
			m_RebuildMaterial = true;
		}

		ImGui::NextColumn();
		ImGui::Text("Shading");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Combo("##Input_Shading", (int*)&m_MatSettings.Shading, "Unlit\0DefaultLit"))
		{
			m_RebuildMaterial = true;
		}

		ImGui::NextColumn();
		ImGui::Text("Two Sided");
		ImGui::NextColumn();
		if (ImGui::Checkbox("##Input_TwoSided", &m_MatSettings.TwoSided))
		{
			m_RebuildMaterial = true;
		}

		ImGui::NextColumn();
		ImGui::Text("Wireframe");
		ImGui::NextColumn();
		if (ImGui::Checkbox("##Input_Wireframe", &m_MatSettings.Wireframe))
		{
			m_RebuildMaterial = true;
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void MaterialEditor::Draw_Errors()
	{
		String Name = String::Format("Compile Errors##MaterialEditor_Errors_%s", GetSuffix().Data());
		ImGui::Begin(Name.Data());

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		for (auto& Error : m_CompileErrors)
		{
			ImGui::Text(Error.Data());
		}
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void MaterialEditor::UpdatePreviewMesh()
	{
		AssetRef<StaticMesh> Mesh;

		switch (PreviewMesh)
		{
		case 0:	// Plane
			Mesh = StaticMeshFactory::Plane(Vec2f(1.0f, 1.0f));
			break;
		default:
		case 1:	// Cube
			Mesh = StaticMeshFactory::Cube(Vec3f(1.0f, 1.0f, 1.0f));
			break;
		case 2:	// UVSphere
			Mesh = StaticMeshFactory::UVSphere();
			break;
		case 3:	// IcoSphere
			//Mesh = StaticMeshFactory::IcoSphere();
			break;
		case 4:	// QuadSphere
			//Mesh = StaticMeshFactory::QuadSphere();
			break;
		case 5:	// Cylinder
			//Mesh = StaticMeshFactory::Cylinder();
			break;
		case 6:	// Capsule
			//Mesh = StaticMeshFactory::Capsule();
			break;
		case 7:	// Pyramid
			//Mesh = StaticMeshFactory::Pyramid();
			break;
		case 8:	// Custom
			break;
		}

		if (m_PreviewMeshComponent != nullptr)
		{
			m_PreviewMeshComponent->SetStaticMesh(Mesh);
			m_PreviewMeshComponent->SetMaterial(m_Material);
		}
	}

	void MaterialEditor::RebuildMaterial()
	{
		m_RebuildMaterial = false;

		Array<ShaderReflection_Variable> Parameters = m_Material->DynamicParameters();
		Array<char> DynamicParameterValues = m_Material->DynamicParameterValues();

		m_CompileErrors.Clear();
		SharedPtr<Material> NewMaterial = SharedPtr<Material>(new Material(m_MatSettings, &m_CompileErrors));
		if (NewMaterial->IsValid())
		{
			const Array<ShaderReflection_Variable>& NewParameters = NewMaterial->DynamicParameters();

			bool ValuesEqual = NewParameters.Size() == Parameters.Size();
			if (ValuesEqual)
			{
				for (size_t i = 0; i < NewParameters.Size(); i++)
				{
					ShaderReflection_Variable Var1 = Parameters[i];
					ShaderReflection_Variable Var2 = NewParameters[i];

					if (Var1.Size != Var2.Size)
					{
						ValuesEqual = false;
						break;
					}
				}

				if (ValuesEqual)
				{
					size_t Offset = 0;
					for (auto& Param : NewParameters)
					{
						NewMaterial->SetParameter(Param.Name, DynamicParameterValues.Data() + Offset, Param.Size, false);
						Offset += Param.Size;
					}
				}
			}

			if (!ValuesEqual)
			{
				size_t Offset = 0;
				for (auto& Param : Parameters)
				{
					NewMaterial->SetParameter(Param.Name, DynamicParameterValues.Data() + Offset, Param.Size, false);
					Offset += Param.Size;
				}
			}

			NewMaterial->UploadParametersToGPU();

			m_Material = NewMaterial;
			m_PreviewMeshComponent->SetMaterial(m_Material);
		}
	}
}
