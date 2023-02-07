#include "CardinalEngine-Editor/UI/Windows/StatisticsDisplay.h"

#include "CardinalEngine/Engine/Engine.h"

#include "CardinalEngine/Framework/ModuleSystem/ModuleRegister.h"
#include "CardinalEngine/Framework/ModuleSystem/Module.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/FileSystem/Paths.h"

#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"

#include "CardinalEngine/Threading/TaskSystem.h"
#include "CardinalEngine/AssetSystem/AssetSystem.h"
#include "CardinalEngine/AssetSystem/AssetRef.h"
#include "CardinalEngine/AssetSystem/AssetStorage.h"
#include "CardinalEngine/AssetSystem/AssetStorage_Directory.h"

#include "CardinalEngine/Game/GameInstance.h"
#include "CardinalEngine/Game/World.h"

#include "CardinalEngine-Editor/ImGuiInterface/imgui/imgui.h"
#include "CardinalEngine-Editor/ImGuiInterface/imgui/imgui_internal.h"

#include <numeric>

namespace CE
{
	void StatisticsDisplay::Draw(float DeltaTime)
	{
		ImGui::Begin("Statistics");

		Draw_General(DeltaTime);
		ImGui::Separator();
		Draw_Modules(DeltaTime);
		ImGui::Separator();
		Draw_Reflection(DeltaTime);
		ImGui::Separator();
		Draw_Paths(DeltaTime);
		ImGui::Separator();
		Draw_Rendering(DeltaTime);
		ImGui::Separator();
		Draw_TaskSystem(DeltaTime);
		ImGui::Separator();
		Draw_AssetSystem(DeltaTime);
		ImGui::Separator();
		Draw_GameInstances(DeltaTime);

		ImGui::End();
	}

	void StatisticsDisplay::Draw_General(float DeltaTime)
	{
		static float Average = 1.0f;
		static std::vector<float> FrameTimes;
		FrameTimes.push_back(DeltaTime * 1000.0f);


		static auto TimeToPrint = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
		if (TimeToPrint <= std::chrono::steady_clock::now())
		{
			TimeToPrint = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
			Average = std::accumulate(FrameTimes.begin(), FrameTimes.end(), 0.0f) / (float)FrameTimes.size();
			FrameTimes.clear();
		}


		if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Average Frame Time: %f ms (%i FPS)", Average, (int)(1000.0f / Average));
		}
	}

	void StatisticsDisplay::Draw_Modules(float DeltaTime)
	{
		const Array<Module*>& ActiveModules = ModuleRegister::Get().ActiveModules();
		String ModuleHeader = String::Format("Modules (%llu)###Modules", ActiveModules.Size());
		if (ImGui::CollapsingHeader(ModuleHeader.Data(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& Module : ActiveModules)
			{
				if (ImGui::TreeNode(Module->Name().Data()))
				{
					VersionNumber Ver = Module->Version();
					ImGui::Text("Version: %i.%i.%i", Ver.Major, Ver.Minor, Ver.Patch);

					std::filesystem::path Path = Platform::Get().GetLibraryPath(Module->GetLibraryHandle());
					ImGui::Text("Path: %s", Path.u8string().data());

					ImGui::TreePop();
				}
			}
		}
	}

	void StatisticsDisplay::Draw_Reflection(float DeltaTime)
	{
		if (ImGui::CollapsingHeader("Reflection", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Map<ReflectionID, Type> AllTypes = Type::All();
			if (ImGui::TreeNode(String::Format("Types (%llu)###Types", AllTypes.size()).Data()))
			{
				ImGui::BeginTable("TypeFilter", 3);
				ImGui::TableNextColumn();
				ImGui::Selectable("Void", &ShowCategory[(int)ETypeCategory::Void]);
				ImGui::TableNextColumn();
				ImGui::Selectable("Fundamental", &ShowCategory[(int)ETypeCategory::Fundamental]);
				ImGui::TableNextColumn();
				ImGui::Selectable("Class", &ShowCategory[(int)ETypeCategory::Class]);
				ImGui::TableNextColumn();
				ImGui::Selectable("Enum", &ShowCategory[(int)ETypeCategory::Enum]);
				ImGui::TableNextColumn();
				ImGui::Selectable("Pointer", &ShowCategory[(int)ETypeCategory::Pointer]);
				ImGui::TableNextColumn();
				ImGui::Selectable("Reference", &ShowCategory[(int)ETypeCategory::Reference]);
				ImGui::EndTable();

				ImGui::Separator();
				for (auto& Type : AllTypes)
				{
					ETypeCategory Category = Type.second.Category();
					if (ShowCategory[(int)Category])
					{
						if (ImGui::TreeNodeEx(Type.second.TypeName().Data(), ImGuiTreeNodeFlags_SpanAvailWidth))
						{
							ImGui::Text("ID: %llu", Type.second.ID().Hash());

							const char* CategoryStr = "UNKNOWN";
							switch (Category)
							{
							case CE::ETypeCategory::Void:
								CategoryStr = "Void";
								break;
							case CE::ETypeCategory::Fundamental:
								CategoryStr = "Fundamental";
								break;
							case CE::ETypeCategory::Class:
								CategoryStr = "Class";
								break;
							case CE::ETypeCategory::Enum:
								CategoryStr = "Enum";
								break;
							case CE::ETypeCategory::Pointer:
								CategoryStr = "Pointer";
								break;
							case CE::ETypeCategory::Reference:
								CategoryStr = "Reference";
								break;
							}
							ImGui::Text("Category: %s", CategoryStr);
							ImGui::Text("Size: %llu", Type.second.Size());
							ImGui::Text("Underlying: %s", Type.second.Underlying().TypeName().Data());

							if (ImGui::TreeNodeEx(String::Format("Bases (%llu)", Type.second.BaseCount()).Data()))
							{
								auto Bases = Type.second.Bases();
								for (auto& Base : Bases)
								{
									ImGui::Text(Base.TypeName().Data());
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNodeEx(String::Format("Constructors (%llu)", Type.second.ConstructorCount()).Data()))
							{
								auto Constructors = Type.second.Constructors();
								uint32 Index = 0;
								for (auto& Constructor : Constructors)
								{
									Index++;
									Draw_Method(Constructor, String::Format("Constructor %u", Index));
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNodeEx(String::Format("Methods (%llu)", Type.second.MethodCount()).Data()))
							{
								auto Methods = Type.second.Methods();
								for (auto& Method : Methods)
								{
									Draw_Method(Method.second);
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNodeEx(String::Format("Properties (%llu)", Type.second.PropertyCount()).Data()))
							{
								auto Properties = Type.second.Properties();
								for (auto& Property : Properties)
								{
									Draw_Property(Property.second);
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNodeEx(String::Format("Enum Entries (%llu)", Type.second.EnumEntryCount()).Data()))
							{
								// List Enum Entries
								ImGui::TreePop();
							}

							if (ImGui::TreeNodeEx(String::Format("Metadatas (%llu)", Type.second.MetadataCount()).Data()))
							{
								// List Enum Entries
								ImGui::TreePop();
							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}


			MultiMap<ReflectionID, Method> GlobalMethods = Method::All();
			if (ImGui::TreeNode(String::Format("Global Methods (%llu)###Global Methods", GlobalMethods.size()).Data()))
			{

				ImGui::TreePop();
			}


			Map<ReflectionID, Property> GlobalProperties = Property::All();
			if (ImGui::TreeNode(String::Format("Global Properties (%llu)###Global Properties", GlobalProperties.size()).Data()))
			{

				ImGui::TreePop();
			}
		}
	}

	void StatisticsDisplay::Draw_Paths(float DeltaTime)
	{
		if (ImGui::CollapsingHeader("Paths"))
		{
			ImGui::Text("Root: %s", Paths::RootDirectory().u8string().data());
			ImGui::Text("Application: %s", Paths::ApplicationDirectory().u8string().data());
			ImGui::Text("Engine Resources: %s", Paths::EngineResources().u8string().data());
			ImGui::Text("Project Work: %s", Paths::ProjectWorkDirectory().u8string().data());
			ImGui::Text("Work: %s", Paths::WorkDirectory().u8string().data());
			ImGui::Text("Project: %s", Paths::ProjectDirectory().u8string().data());
			ImGui::Text("Editor Resources: %s", Paths::EditorResources().u8string().data());
			ImGui::Text("Editor Work: %s", Paths::EditorWorkDirectory().u8string().data());
		}
	}

	void StatisticsDisplay::Draw_Rendering(float DeltaTime)
	{
		DynamicRHI& RHI = Engine::Get().DynamicRHI();

		const char* RenderStr = "Unknown";
		switch (RHI.Type())
		{
		case CE::ERHIType::None:
			RenderStr = "None";
			break;
		case CE::ERHIType::Vulkan:
			RenderStr = "Vulkan";
			break;
		case CE::ERHIType::DirectX12:
			RenderStr = "DirectX12";
			break;
		case CE::ERHIType::DirectX11:
			RenderStr = "DirectX11";
			break;
		default:
			RenderStr = "Unknown";
			break;
		}

		if (ImGui::CollapsingHeader(String::Format("Rendering (%s)", RenderStr).Data(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::TreeNode("Config"))
			{

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Capabilities"))
			{
				ImGui::Text("Device: %S", RHI.Capabilities().DeviceName.Data());
				ImGui::TreePop();
			}

			ImGui::Text("Used Memory: 0 / 0 MB");
			ImGui::Text("Deferred Contexts: %u", 0);
			ImGui::Text("Graphic Pipelines: %u", 0);
			ImGui::Text("Compute Pipelines: %u", 0);
			ImGui::Text("RayTracing Pipelines: %u", 0);
			ImGui::Text("Draw Calls Last Frame: %u", 0);
			ImGui::Text("Vertices Last Frame: %u", 0);
		}
	}

	void StatisticsDisplay::Draw_TaskSystem(float DeltaTime)
	{
		if (ImGui::CollapsingHeader("Task System", ImGuiTreeNodeFlags_DefaultOpen))
		{
			uint32 Working = 0;
			size_t Remaining = 0;
			Engine::Get().TaskSystem.QueueStats(EThreadType::Worker, Remaining, Working);

			ImGui::Text("Worker Working: %u", Working);
			ImGui::Text("Remaining Worker Tasks: %llu", Remaining);
		}
	}

	void StatisticsDisplay::Draw_AssetSystem(float DeltaTime)
	{
		if (ImGui::CollapsingHeader("Asset System", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::TreeNodeEx("Asset Importers"))
			{
				ImGui::Columns(2);
				ImGui::Text("Name");
				ImGui::NextColumn();
				ImGui::Text("Extensions");
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Columns(2);

				for (auto& Importer : Engine::Get().AssetSystem.GetAllImporters())
				{
					Array<String> Extensions = Importer.second->Extensions();
					ImGui::Text("%s", Importer.second->Name().Data());
					ImGui::NextColumn();
					bool IsFirst = true;
					String ExtensionsStr;
					for (auto& Extension : Extensions)
					{
						if (IsFirst)
						{
							IsFirst = false;
						}
						else
						{
							ExtensionsStr += ", ";
						}
						ExtensionsStr += Extension;
					}
					ImGui::Text(ExtensionsStr.Data());
					ImGui::NextColumn();
				}
				ImGui::Columns(1);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx(String::Format("Asset Storages (%llu)###AssetStorages", Engine::Get().AssetSystem.GetAssetStorages().size()).Data()))
			{
				auto& AssetStorages = Engine::Get().AssetSystem.GetAssetStorages();
				for (auto& StorageIt : AssetStorages)
				{
					AssetStorage* Storage = StorageIt.second.Get();
					if (ImGui::TreeNode(Storage->ID.Data()))
					{
						ImGui::Text("Hash: %llu", Storage->Hash);
						ImGui::Text("Path: %s", Storage->Path.u8string().c_str());
						const AssetStorage_Directory* ASD = dynamic_cast<const AssetStorage_Directory*>(Storage);
						/*
						const AssetStorage_Archive* ASA = dynamic_cast<const AssetStorage_Archive*>(Storage);
						if (ASA != nullptr)
						{
							ImGui::Text("Type: Archive");
						}
						else*/ if (ASD != nullptr)
						{
							ImGui::Text("Type: Directory");
						}
						else
						{
							ImGui::Text("Type: Unknown");
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx(String::Format("Loaded Assets (%llu)###LoadedAssets", Engine::Get().AssetSystem.LoadedAssets().size()).Data()))
			{
				Map<uint64, String> StorageNames;

				ImGuiTableFlags Flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable;
				if (ImGui::BeginTable("##LoadedAssets", 7, Flags))
				{
					ImGui::TableSetupColumn("Path");
					ImGui::TableSetupColumn("Storage");
					ImGui::TableSetupColumn("State");
					ImGui::TableSetupColumn("Type");
					ImGui::TableSetupColumn("UseCount");
					ImGui::TableSetupColumn("RefCount");
					ImGui::TableHeadersRow();

					for (auto& Asset : Engine::Get().AssetSystem.LoadedAssets())
					{
						ImGui::TableNextRow();

						AssetRefData& Info = *Asset.second.Asset.Get();
						EAssetState State = EAssetState::Empty;
						String ClassName = "Unknown";
						size_t UseCount = 0;
						{
							std::lock_guard<Spinlock> Lock(Info.Lock);
							if (Info.LoadState.valid())
							{
								State = EAssetState::Loading;
								if (Info.LoadState._Is_ready())
								{
									State = Info.LoadState.get() ? EAssetState::Ready : EAssetState::Error;
								}
							}
							if (Info.Asset != nullptr)
							{
								ClassName = Info.Asset->GetType().TypeName();
							}
							UseCount = Info.Asset.UseCount();
						}

						ImGui::TableNextColumn();
						ImGui::Text(Asset.first.Path().u8string().c_str());

						ImGui::TableNextColumn();
						uint64 StorageHash = Asset.first.StorageHash();
						auto Found = StorageNames.find(StorageHash);
						if (Found == StorageNames.end())
						{
							SharedPtr<AssetStorage> Storage = Engine::Get().AssetSystem.GetAssetStorage(StorageHash);
							Found = StorageNames.emplace(StorageHash, Storage != nullptr ? Storage->ID : "").first;
						}
						ImGui::Text(Found->second.Data());

						ImGui::TableNextColumn();
						switch (State)
						{
						default:
							ImGui::Text("Unknown");
							break;
						case EAssetState::Empty:
							ImGui::Text("Empty");
							break;
						case EAssetState::Loading:
							ImGui::Text("Loading");
							break;
						case EAssetState::Error:
							ImGui::Text("Error");
							break;
						case EAssetState::Ready:
							ImGui::Text("Ready");
							break;
						}

						ImGui::TableNextColumn();
						ImGui::Text(ClassName.Data());

						ImGui::TableNextColumn();
						ImGui::Text("%llu", UseCount);

						ImGui::TableNextColumn();
						ImGui::Text("%llu", Asset.second.Asset.UseCount());

						ImGui::TableNextColumn();
						if (ImGui::Button(String::Format("Reload##%llu:%s", StorageHash, Asset.first.Path().u8string().c_str()).Data()))
						{
							Engine::Get().AssetSystem.Load(Asset.first, true);
						}
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
	}

	void StatisticsDisplay::Draw_GameInstances(float DeltaTime)
	{
		if (ImGui::CollapsingHeader(String::Format("Game Instances (%llu)###Game Instances", Engine::Get().GameInstances().Size()).Data(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& GameInstance : Engine::Get().GameInstances())
			{
				if (ImGui::TreeNode(String::Format("%p (%s)", GameInstance.Get(), typeid(*GameInstance.Get()).name()).Data()))
				{
					if (ImGui::TreeNode(String::Format("Worlds (%llu)##Worlds", GameInstance->Worlds().Size()).Data()))
					{
						for (auto& World : GameInstance->Worlds())
						{
							if (ImGui::TreeNode(String::Format("%s##%p", World->EditorName.Data(), World.Get()).Data()))
							{
								ImGui::Text("Entities: %llu", World->Entities().Size());
								ImGui::Text("Components: %llu", World->ComponentCount.load());
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Players (0)##Players"))
					{

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}

			}
		}
	}

	void StatisticsDisplay::Draw_Method(const Method& Method, const String& AlternativeName)
	{
		String Name = Method.Name();
		if (Name.Size() == 0)
		{
			Name = AlternativeName;
		}
		if (ImGui::TreeNodeEx(Name.Data()))
		{
			ImGui::Text("ID: %llu", Method.ID().Hash());
			ImGui::Text("Static: %s", Method.IsStatic() ? "True" : "False");

			if (ImGui::TreeNodeEx(String::Format("Arguments (%llu)", Method.ArgumentCount()).Data()))
			{
				auto Arguments = Method.ArgumentTypes();
				for (auto& Argument : Arguments)
				{
					ImGui::Text(Argument.TypeName().Data());
				}
				ImGui::TreePop();
			}

			ImGui::Text("Return: ", Method.ReturnType().TypeName().Data());
			ImGui::Text("Owner: ", Method.Owner().TypeName().Data());

			// Metadatas
			ImGui::TreePop();
		}
	}

	void StatisticsDisplay::Draw_Property(const Property& Property)
	{

	}
}
