#include "CardinalEngine-Editor/UI/Windows/AssetBrowser.h"
#include "CardinalEngine/AssetSystem/AssetSystem.h"
#include "CardinalEngine/AssetSystem/AssetStorage.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/AssetSystem/AssetImporter.h"

#include "CardinalEngine-Editor/EngineEditor/EditorGameInstance.h"

#include "CardinalEngine/Material/Material.h"
#include "CardinalEngine-Editor/AssetEditors/MaterialEditor.h"

#include "CardinalEngine/Mesh/StaticMesh.h"

#include "CardinalEngine/FileSystem/Paths.h"

namespace CE
{
	AssetBrowser::AssetBrowser()
	{
		auto& AS = Engine::Get().AssetSystem;

		Icon_Unknown = AS.Get<Texture>(AssetID("CardinalEngine-Editor", "Assets/Icons/QuestionMark"));
		Icon_Directory = AS.Get<Texture>(AssetID("CardinalEngine-Editor", "Assets/Icons/Folder"));
		Icon_Asset[Texture::StaticType()] = AS.Get<Texture>(AssetID("CardinalEngine-Editor", "Assets/Icons/Texture"));
		Icon_Asset[Material::StaticType()] = AS.Get<Texture>(AssetID("CardinalEngine-Editor", "Assets/Icons/Material"));
	}

	void AssetBrowser::Draw(float DeltaTime)
	{
		CE_PROFILE_FUNCTION_HIERARCHY();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Asset Browser", &IsOpen);
		ImGui::PopStyleVar();
		ImGui::Columns(2);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y);
		DrawHierarchyHeader();
		ImGui::NextColumn();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y);
		DrawDirectoryHeader();
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::BeginChild("Hierarchy");
		DrawHierarchy();
		ImGui::EndChild();
		ImGui::NextColumn();
		ImGui::BeginChild("Directory");
		DrawDirectory();
		ImGui::EndChild();

		ImGui::Columns(1);
		ImGui::End();

		DrawImportPopup();
	}

	void AssetBrowser::DrawHierarchyHeader()
	{
		SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
		ImGui::BeginDisabled(AS == nullptr);

		ImGui::Button("New");
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetCurrentWindow()->Pos.x + ImGui::GetCursorPos().x, ImGui::GetCurrentWindow()->Pos.y + ImGui::GetCursorPos().y));
		if (ImGui::BeginPopupContextItem("NewAssetMenu", ImGuiMouseButton_Left))
		{
			if (ImGui::MenuItem("Directory"))
			{
				std::filesystem::path Path = SelectedPath;
				Path += "/New Directory";
				Path = Path.lexically_normal();
				if (AS->CreateDirectories(Path))
				{
					m_Rename = true;
					m_NewName = "New Directory";
				}
			}
			if (ImGui::MenuItem("Material"))
			{
				std::filesystem::path Path = SelectedPath;
				Path += "/New Material";
				Path = Path.lexically_normal();

				MaterialSettings DefaultMatSettings;
				DefaultMatSettings.ShaderSource = R"(void CalculateMaterial_Vertex(inout Material OutMaterial, Material_InputVertex Input)
{

}

void CalculateMaterial_Fragment(inout Material OutMaterial, Material_InputFragment Input)
{

}
)";
				SharedPtr<Material> NewMaterial = SharedPtr<Material>(new Material(DefaultMatSettings));
				if (Engine::Get().AssetSystem.Store(NewMaterial, AssetID(AS->Hash, Path)))
				{
					m_Rename = true;
					m_NewName = "New Material";
				}
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Import..."))
		{
			Set<String> Extensions;

			auto& Importers = Engine::Get().AssetSystem.GetExtensionImporters();
			for (auto& Importer : Importers)
			{
				Extensions.insert(Importer.first);
			}

			Text ExtensionsStr;
			Text ExtensionsPretty;
			for (auto& Extension : Extensions)
			{
				if (ExtensionsStr.Size() != 0)
				{
					ExtensionsStr += L";";
				}
				if (ExtensionsPretty.Size() != 0)
				{
					ExtensionsPretty += L", ";
				}
				ExtensionsStr += L"*.";
				Text ExtensionText = Text::Convert(Extension);
				ExtensionsStr += ExtensionText;
				ExtensionsPretty += ExtensionText;
			}

			Array<std::filesystem::path> ImportPaths;
			if (Platform::Get().ShowOpenFileDialog(ImportPaths, L"Import Asset", { {ExtensionsPretty, ExtensionsStr} }, EFileDialogFlag::AllowMulti | EFileDialogFlag::FileMustExist | EFileDialogFlag::DontAddToRecent))
			{
				m_Imports.Clear();
				for (auto& Path : ImportPaths)
				{
					m_Imports.Add({ Path });
				}
			}
		}

		ImGui::SameLine();
		ImVec2 ButtonSize = ImGui::CalcButtonSize("O");
		if (ButtonSize.x < ImGui::GetContentRegionAvail().x)
		{
			ImGui::SetCursorPosX((ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x) - ButtonSize.x);
		}
		ImGui::EndDisabled();

		if (ImGui::Button("O"))
		{
			RefreshAllStorages();
		}
	}

	void AssetBrowser::DrawHierarchy()
	{
		for (auto& AS : Engine::Get().AssetSystem.GetAssetStorages())
		{
			ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool IsSelected = SelectedStorage.Lock() == AS.second && SelectedPath.empty();
			if (IsSelected)
			{
				Flags |= ImGuiTreeNodeFlags_Selected;
			}
			bool IsOpen = ImGui::TreeNodeEx(AS.second->ID.Data(), Flags);
			if (ImGui::IsItemActivated() && !ImGui::IsItemToggledOpen())
			{
				SelectedStorage = AS.second;
				SelectedPath = "";
				m_Selected.clear();
				m_Rename = false;
			}

			if (IsOpen)
			{
				const Map<std::filesystem::path, AssetHeader>& Headers = AS.second->AssetHeaders();
				for (auto It = Headers.begin(); It != Headers.end();)
				{
					if (It->second.AssetType == Type::Get<void>())
					{
						DrawHierarchyDirectory(AS.second, SelectedStorage.Lock() == AS.second, Headers, It, It->first);
					}
					else
					{
						It++;
					}
				}

				ImGui::TreePop();
			}
		}
	}

	void AssetBrowser::DrawHierarchyDirectory(const SharedPtr<AssetStorage>& AS, bool IsStorageSelected, const Map<std::filesystem::path, AssetHeader>& Headers, Map<std::filesystem::path, AssetHeader>::const_iterator& It, const std::filesystem::path& Path)
	{
		ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		bool IsSelected = IsStorageSelected && SelectedPath == Path;
		if (IsSelected)
		{
			Flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool IsOpen = ImGui::TreeNodeEx(Path.filename().u8string().c_str(), Flags);
		if (ImGui::IsItemActivated() && !ImGui::IsItemToggledOpen())
		{
			SelectedStorage = AS;
			SelectedPath = Path;
			m_Selected.clear();
			m_Rename = false;
		}

		It++;
		while (It != Headers.end())
		{
			if (AssetStorage::IsSubdirectoryOf(It->first, Path))
			{
				if (IsOpen && It->second.AssetType == Type::Get<void>())
				{
					DrawHierarchyDirectory(AS, IsStorageSelected, Headers, It, It->first);
				}
				else
				{
					It++;
				}
			}
			else
			{
				break;
			}
		}

		if (IsOpen)
		{
			ImGui::TreePop();
		}
	}

	void AssetBrowser::DrawDirectoryHeader()
	{
		ImGui::SetNextItemWidth(150.0f);
		ImGui::InputTextWithHint("##Search", "Search...", &m_Search);

		ImGui::SameLine();

		ImGui::SetNextItemWidth(70.0f);
		ImGui::Combo("##DisplayType", (int*)&DisplayType, "Tiles\0List");

		if (DisplayType == EDisplayType::Tiles)
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::Combo("##TileSize", (int*)&TileSize, "Tiny\0Small\0Normal\0Big\0Large");
		}
	}

	void AssetBrowser::DrawDirectory()
	{
		SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
		if (AS != nullptr)
		{
			auto& AssetHeaders = AS->AssetHeaders();

			switch (DisplayType)
			{
			default:
			case EDisplayType::Tiles:
			{
				float MinEntryWidth = 0.0f;
				switch (TileSize)
				{
				case ETileSize::Tiny:
					MinEntryWidth = 75.0f;
					break;
				case ETileSize::Small:
					MinEntryWidth = 85.0f;
					break;
				default:
				case ETileSize::Normal:
					MinEntryWidth = 95.0f;
					break;
				case ETileSize::Big:
					MinEntryWidth = 105.0f;
					break;
				case ETileSize::Large:
					MinEntryWidth = 115.0f;
					break;
				}
				float Width = ImGui::GetContentRegionAvail().x;
				int Columns = std::max(int(Width / MinEntryWidth), 1);

				if (ImGui::BeginTable("Items", Columns, ImGuiTableFlags_NoPadOuterX))
				{
					size_t Index = 0;
					for (auto& It : AssetHeaders)
					{
						if (SelectedPath == It.first.parent_path())
						{
							if (Index % Columns == 0)
							{
								ImGui::TableNextRow();
							}
							ImGui::TableNextColumn();
							DrawDirectoryItem(It.second, It.first, Index);
							Index++;
						}
					}

					ImGui::EndTable();
				}
				break;
			}
			case EDisplayType::List:
			{
				uint32 ColumnCount = 2;
				if (ImGui::BeginTable("table_scrollx", ColumnCount, ImGuiTableFlags_Resizable))
				{
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Type");
					ImGui::TableHeadersRow();
					size_t Index = 0;
					for (auto& It : AssetHeaders)
					{
						if (SelectedPath == It.first.parent_path())
						{
							ImGui::TableNextRow();
							DrawDirectoryItem(It.second, It.first, Index);
							Index++;
						}
					}
					ImGui::EndTable();
				}
				break;
			}
			}
		}
	}

	void AssetBrowser::DrawDirectoryItem(const AssetHeader& Header, const std::filesystem::path& Path, uint32 Index)
	{
		String Title = Path.stem().u8string();
		const char* ID = Title.Data();
		bool IsDirectory = Header.AssetType == Type::Get<void>();
		bool Selected = m_Selected.find(Path) != m_Selected.end();

		switch (DisplayType)
		{
		default:
		case EDisplayType::Tiles:
		{
			float Width = ImGui::GetContentRegionAvail().x;
			float Height = Width * 1.5f;
			ImGui::BeginChild(ID, ImVec2(Width, Height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushID(ID);
			ImGui::BeginGroup();

			auto* DrawList = ImGui::GetWindowDrawList();
			ImRect ItemRect = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + Width, ImGui::GetCursorScreenPos().y + Height));

			// Background
			uint32 BackgroundColor = ImGui::GetColorU32(ImGuiCol_Header);
			DrawList->AddRectFilled(ItemRect.Min, ItemRect.Max, BackgroundColor);

			// Icon Background
			DrawList->AddRectFilled(ItemRect.Min, ImVec2(ImGui::GetCursorScreenPos().x + Width, ImGui::GetCursorScreenPos().y + Width), IM_COL32(30, 30, 30, 255));

			// Border
			if (Selected || ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID)))
			{
				uint32 BorderColor = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
				DrawList->AddRect(ItemRect.Min, ItemRect.Max, BorderColor, 0.0f, 0, 2.5f);
			}

			AssetRef<Texture> IconAsset;
			if (IsDirectory)
			{
				IconAsset = Icon_Directory;
			}
			else
			{
				auto FoundAsset = Icon_Asset.find(Header.AssetType);
				if (FoundAsset != Icon_Asset.end())
				{
					IconAsset = FoundAsset->second;
				}
				else
				{
					IconAsset = Icon_Unknown;
				}
			}


			if (IconAsset.State() == EAssetState::Ready)
			{
				float TexturePadding = 5.0f;

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + TexturePadding);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TexturePadding);

				SharedPtr<Texture> Texture = IconAsset.Get();
				ImGui::Image(Texture->Image(), ImVec2(Width - (TexturePadding * 2.0f), Width - (TexturePadding * 2.0f)));
			}
			else
			{
				ImRect IconRect = ItemRect;
				IconRect.Max.y = IconRect.Min.y + Width;
				ImGui::ItemSize(IconRect);
			}

			String Name = Path.filename().replace_extension().u8string().c_str();

			if (Selected && m_Rename)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::SetKeyboardFocusHere();
				if (ImGui::InputText("##Rename", &m_NewName, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
					CE_CHECK(AS != nullptr);

					AS->Rename(Path, m_NewName);
					m_Rename = false;
				}

				if (ImGui::IsItemDeactivated())
				{
					m_Rename = false;
				}
			}
			else
			{
				if (IsDirectory)
				{
					ImGui::TextWrapped(Name.Data());
				}
				else
				{
					ImGui::TextWrapped(Name.Data());
				}
			}

			if (ImGui::ItemHoverable(ItemRect, ImGui::GetID(ID)))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (IsDirectory)
					{
						SelectedPath = Path;
						m_Selected.clear();
						m_Rename = false;
					}
					else
					{
						OpenAssetEditor(SelectedStorage.Lock(), Path, Header);
					}
				}
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (m_Selected.find(Path) != m_Selected.end())
						{
							m_Selected.erase(Path);
						}
						else
						{
							m_Selected.insert(Path);
						}
					}
					else
					{
						m_Selected = { Path };
					}
					m_Rename = false;
				}
			}

			ImGui::EndGroup();
			ImGui::PopID();
			ImGui::EndChild();

			if (ImGui::BeginPopupContextItem(ID))
			{
				if (m_Selected.find(Path) == m_Selected.end())
				{
					m_Selected = { Path };
					m_Rename = false;
				}

				bool Many = m_Selected.size() > 1;
				ImGui::BeginDisabled(Many);
				if (ImGui::MenuItem("Rename"))
				{
					m_Rename = true;
					m_NewName = Name;
				}
				ImGui::EndDisabled();
				ImGui::MenuItem(Many ? "Copy All" : "Copy");
				if (ImGui::MenuItem(Many ? "Delete All" : "Delete"))
				{
					SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
					CE_CHECK(AS != nullptr);

					for (auto& Selected : m_Selected)
					{
						AS->Delete(Selected);
					}
				}

				if (!IsDirectory)
				{
					ImGui::Separator();
					if (ImGui::MenuItem("Load/Reload"))
					{
						SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
						CE_CHECK(AS != nullptr);

						std::filesystem::path AssetPath = Path;
						AssetPath.replace_extension("");
						if (!Engine::Get().AssetSystem.Load(AssetID(AS->Hash, AssetPath), true))
						{
							CE_LOG(ELog::Warning, "Failed to load Asset '%s::%s'", AS->ID.Data(), AssetPath.u8string().c_str());
						}
					}
				}

				ImGui::EndPopup();
			}

			break;
		}
		case EDisplayType::List:
		{
			ImGui::TableNextColumn();
			ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
			String Name = Path.filename().replace_extension("").u8string();
			if (Selected && m_Rename)
			{
				ImGui::SetKeyboardFocusHere();
				if (ImGui::InputText("##Rename", &m_NewName, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
					CE_CHECK(AS != nullptr);

					AS->Rename(Path, m_NewName);
					m_Rename = false;
				}

				if (ImGui::IsItemDeactivated())
				{
					m_Rename = false;
				}
			}
			else
			{
				if (ImGui::Selectable(Name.Data(), &Selected, ImGuiSelectableFlags_SpanAllColumns))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (m_Selected.find(Path) != m_Selected.end())
						{
							m_Selected.erase(Path);
						}
						else
						{
							m_Selected.insert(Path);
						}
					}
					else
					{
						m_Selected = { Path };
					}
					m_Rename = false;
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (m_Selected.find(Path) == m_Selected.end())
					{
						m_Selected = { Path };
						m_Rename = false;
					}

					bool Many = m_Selected.size() > 1;
					ImGui::BeginDisabled(Many);
					if (ImGui::MenuItem("Rename"))
					{
						m_Rename = true;
						m_NewName = Name;
					}
					ImGui::EndDisabled();
					ImGui::MenuItem(Many ? "Copy All" : "Copy");
					if (ImGui::MenuItem(Many ? "Delete All" : "Delete"))
					{
						SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
						CE_CHECK(AS != nullptr);

						for (auto& Selected : m_Selected)
						{
							AS->Delete(Selected);
						}
					}

					if (!IsDirectory)
					{
						ImGui::Separator();
						if (ImGui::MenuItem("Load/Reload"))
						{
							SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
							CE_CHECK(AS != nullptr);

							std::filesystem::path AssetPath = Path;
							AssetPath.replace_extension("");
							if (!Engine::Get().AssetSystem.Load(AssetID(AS->Hash, AssetPath), true))
							{
								CE_LOG(ELog::Warning, "Failed to load Asset '%s::%s'", AS->ID.Data(), AssetPath.u8string().c_str());
							}
						}
					}

					ImGui::EndPopup();
				}
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (IsDirectory)
				{
					SelectedPath = Path;
					m_Selected.clear();
					m_Rename = false;
				}
				else
				{
					OpenAssetEditor(SelectedStorage.Lock(), Path, Header);
				}
			}

			ImGui::TableNextColumn();
			if (IsDirectory)
			{
				ImGui::Text("Directory");
			}
			else
			{
				if (Header.AssetType.IsValid())
				{
					ImGui::Text(Header.AssetType.TypeName().Data());
				}
				else
				{
					ImGui::Text("Unknown");
				}
			}
			break;
		}
		}
	}

	void AssetBrowser::DrawImportPopup()
	{
		SharedPtr<AssetStorage> AS = SelectedStorage.Lock();
		if (m_Imports.Size() > 0 && AS != nullptr)
		{
			size_t AssetStorageHash = AS->Hash;

			ImGui::OpenPopup("AssetImport");
			if (ImGui::BeginPopupModal("AssetImport"))
			{
				float WindowHeight = ImGui::GetWindowHeight();

				float ButtonHeight = 25.0f;
				ImGui::BeginChild("ImportList", ImVec2(0.0f, WindowHeight - (ButtonHeight * 2.7f)));
				for (auto& Import : m_Imports)
				{
					if (ImGui::CollapsingHeader(Import.Path.filename().u8string().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Columns(2);
						ImGui::Text("Importer");
						ImGui::NextColumn();

						String Extension = Import.Path.extension().u8string();
						Extension.RemoveAt(0);	//Remove the dot
						SharedPtr<AssetImporter> SelectedImporter = Import.Importer.Lock();
						const Array<SharedPtr<AssetImporter>>& Importers = Engine::Get().AssetSystem.GetAssetImporters(Extension);
						if (Importers.Size() > 0 && !Importers.ContainsByPredicate([&](const SharedPtr<AssetImporter> It) { return It == SelectedImporter; }))
						{
							Import.Importer = Importers[0];
						}

						String PreviewVal = "No Importer Found";
						if (SelectedImporter != nullptr)
						{
							PreviewVal = SelectedImporter->Name();
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::BeginCombo(String::Format("##COMBO_%s", Import.Path.filename().u8string().c_str()).Data(), PreviewVal.Data()))
						{
							for (auto& Importer : Importers)
							{
								bool IsSelected = Importer == SelectedImporter;
								if (ImGui::Selectable(Importer->Name().Data(), &IsSelected))
								{
									Import.Importer = Importer;
								}
							}
							ImGui::EndCombo();
						}
						if (SelectedImporter == nullptr)
						{
							ImGui::PopStyleColor();
						}
						ImGui::Columns(1);
					}
				}
				ImGui::EndChild();

				float ButtonWidth = (ImGui::GetContentRegionAvail().x / 2.0f) - ImGui::GetStyle().FramePadding.x;
				if (ImGui::Button("Confirm", ImVec2(ButtonWidth, ButtonHeight)))
				{
					for (auto& ImportInfo : m_Imports)
					{
						SharedPtr<AssetImporter> Importer = ImportInfo.Importer.Lock();
						if (Importer == nullptr)
						{
							continue;
						}

						SharedPtr<AssetImport> ImportResult = Engine::Get().AssetSystem.Import(Importer, ImportInfo.Path);
						ImportResult->State.wait();	//@TODO: Maybe do a fancy loading screen

						if (ImportResult->State.get() != EAssetImportResult::Success)
						{
							CE_LOG(ELog::Error, "Failed to import Asset '%s' with '%s' importer", ImportInfo.Path.u8string().c_str(), Importer->Name().Data());
							continue;
						}

						// Store Asset To Storage
						uint32 Index = 0;
						for (auto& ImportedAsset : ImportResult->Assets)
						{
							Index++;
							std::filesystem::path NewPath = SelectedPath / ImportInfo.Path.stem();
							if (ImportResult->Assets.Size() > 1)
							{
								NewPath += "_" + std::to_string(Index);
							}

							if (!Engine::Get().AssetSystem.Store(ImportedAsset, AssetID(AssetStorageHash, NewPath)))
							{
								CE_LOG(ELog::Error, "Failed to store Asset to '%s:%s'", AS->ID.Data(), NewPath.u8string().c_str());
							}
						}
					}

					m_Imports.Clear();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(ButtonWidth, ButtonHeight)))
				{
					m_Imports.Clear();
				}
				ImGui::EndPopup();
			}
		}
	}

	void AssetBrowser::RefreshAllStorages()
	{
		for (auto& AS : Engine::Get().AssetSystem.GetAssetStorages())
		{
			AS.second->DiscoverAll();
		}
	}

	void AssetBrowser::OpenAssetEditor(const SharedPtr<AssetStorage>& Storage, const std::filesystem::path& Path, const AssetHeader& Header)
	{
		Array<SharedPtr<EditorWindow>>& EditorWindows = EditorGameInstance::Get()->UI.Windows;

		if (EditorWindows.ContainsByPredicate([&](const SharedPtr<EditorWindow>& It)
			{
				AssetEditor* AE = dynamic_cast<AssetEditor*>(It.Get());
		return AE != nullptr ? AE->StorageHash() == Storage->Hash && AE->Path() == Path : false;
			}))
		{
			return;
		}

			if (Header.AssetType == Material::StaticType())
			{
				EditorWindows.Add(SharedPtr<MaterialEditor>(new MaterialEditor(Storage->Hash, Path)));
			}
	}
}
