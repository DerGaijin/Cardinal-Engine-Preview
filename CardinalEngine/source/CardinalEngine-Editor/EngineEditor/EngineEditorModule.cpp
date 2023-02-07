#include "CardinalEngine-Editor/EngineEditor/EngineEditorModule.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/FileSystem/Paths.h"

namespace CE
{
	String EngineEditorModule::Name() const
	{
		return "Cardinal Engine Editor";
	}

	VersionNumber EngineEditorModule::Version() const
	{
		return VersionNumber(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
	}

	void EngineEditorModule::Register()
	{
		Engine::Get().AssetSystem.AddAssetStorage("CardinalEngine-Editor", Paths::EditorResources());
	}

	void EngineEditorModule::Unregister()
	{
		Engine::Get().AssetSystem.RemoveAssetStorage(String("CardinalEngine-Editor"));
	}

	void EngineEditorModule::RegisterReflection(ReflectionRegistration& Register)
	{

	}

	bool EngineEditorModule::IsCompatibleWith(Module& Module)
	{
		return true;
	}
}
