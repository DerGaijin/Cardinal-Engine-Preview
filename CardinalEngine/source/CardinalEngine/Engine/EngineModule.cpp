#include "CardinalEngine/Engine/EngineModule.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/FileSystem/Paths.h"

#include "CardinalEngine/Framework/Object/Object.h"
#include "CardinalEngine/AssetSystem/Asset.h"
#include "CardinalEngine/AssetSystem/Importers/AssetImporter_stbi.h"
#include "CardinalEngine/AssetSystem/Importers/AssetImporter_tinyobj.h"
#include "CardinalEngine/Material/Texture.h"
#include "CardinalEngine/Material/Material.h"

#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Entity.h"
#include "CardinalEngine/Game/Components/CameraComponent.h"
#include "CardinalEngine/Game/Components/EntityComponent.h"
#include "CardinalEngine/Game/Components/GeometryComponent.h"
#include "CardinalEngine/Game/Components/LightComponent.h"
#include "CardinalEngine/Game/Components/StaticMeshComponent.h"
#include "CardinalEngine/Game/Components/TransformComponent.h"

namespace CE
{
	String EngineModule::Name() const
	{
		return "Cardinal Engine";
	}

	VersionNumber EngineModule::Version() const
	{
		return VersionNumber(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
	}

	void EngineModule::Register()
	{
		Engine::Get().AssetSystem.AddAssetStorage("CardinalEngine", Paths::EngineResources());

		Engine::Get().AssetSystem.AddImporter(SharedPtr<AssetImporter_stbi>(new AssetImporter_stbi()));
		Engine::Get().AssetSystem.AddImporter(SharedPtr<AssetImporter_tinyobj>(new AssetImporter_tinyobj()));
	}

	void EngineModule::Unregister()
	{
		Engine::Get().AssetSystem.RemoveImporter("stb_image");
		Engine::Get().AssetSystem.RemoveImporter("tiny_obj_loader");

		Engine::Get().AssetSystem.RemoveAssetStorage(String("CardinalEngine"));
	}

	void EngineModule::RegisterReflection(ReflectionRegistration& Register)
	{
		Register.Type<void>();
		Register.Type<void*>();

#define REG_PRIM(PRIM) \
		Register.Type<PRIM>(); \
		Register.Type<PRIM*>(); \
		Register.Type<PRIM&>()

		// Primitives
		REG_PRIM(bool);
		REG_PRIM(int8);
		REG_PRIM(int16);
		REG_PRIM(int32);
		REG_PRIM(int64);
		REG_PRIM(uint8);
		REG_PRIM(uint16);
		REG_PRIM(uint32);
		REG_PRIM(uint64);
		REG_PRIM(char);
		REG_PRIM(wchar_t);
		REG_PRIM(float);
		REG_PRIM(double);
		REG_PRIM(long double);

		auto Reg_Object = Register.Class<Object>();
		Reg_Object.Constructor<>();

#pragma region ASSETS
		auto Reg_Asset = Register.Class<Asset>();
		Reg_Asset.Base<Object>();

		auto Reg_Texture = Register.Class<Texture>();
		Reg_Texture.Base<Asset>();
		Reg_Texture.Constructor<>();

		auto Reg_Material = Register.Class<Material>();
		Reg_Material.Base<Asset>();
		Reg_Material.Constructor<>();
#pragma endregion ASSETS

#pragma region GAME
		auto Reg_World = Register.Class<World>();
		Reg_World.Base<Object>();

		auto Reg_Entity = Register.Class<Entity>();
		Reg_Entity.Base<Object>();

		auto Reg_EntityComponent = Register.Class<EntityComponent>();
		Reg_EntityComponent.Base<Object>();

		auto Reg_TransformComponent = Register.Class<TransformComponent>();
		Reg_TransformComponent.Base<EntityComponent>();

		auto Reg_CameraComponent = Register.Class<CameraComponent>();
		Reg_CameraComponent.Base<TransformComponent>();

		auto Reg_GeometryComponent = Register.Class<GeometryComponent>();
		Reg_GeometryComponent.Base<TransformComponent>();

		auto Reg_StaticMeshComponent = Register.Class<StaticMeshComponent>();
		Reg_StaticMeshComponent.Base<GeometryComponent>();

		auto Reg_LightComponent = Register.Class<LightComponent>();
		Reg_LightComponent.Base<TransformComponent>();
#pragma endregion GAME
	}

	bool EngineModule::IsCompatibleWith(Module& Module)
	{
		return true;
	}
}
