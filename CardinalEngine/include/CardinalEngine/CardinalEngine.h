#pragma once
#include "Framework/Core/Headers.h"

#include "Framework/Configs/ConfigParser.h"
#include "Framework/Configs/Ini/IniDocument.h"
#include "Framework/Configs/Ini/IniDocumentParser.h"
#include "Framework/Configs/Ini/IniParser.h"
#include "Framework/Configs/Xml/XmlDocument.h"
#include "Framework/Configs/Xml/XmlDocumentParser.h"
#include "Framework/Configs/Xml/XmlParser.h"

#include "Framework/Delegates/Delegate.h"
#include "Framework/Delegates/DelegateMulti.h"
#include "Framework/Delegates/DelegateSingle.h"

#include "Framework/ModuleSystem/Module.h"
#include "Framework/ModuleSystem/ModuleRegister.h"

#include "Framework/Object/Object.h"

#include "Framework/Platform/Platform.h"

#include "Framework/Reflection/Detail/MethodInfo.h"
#include "Framework/Reflection/Detail/MethodWrappers.h"
#include "Framework/Reflection/Detail/PropertyInfo.h"
#include "Framework/Reflection/Detail/PropertyWrappers.h"
#include "Framework/Reflection/Detail/TypeInfo.h"
#include "Framework/Reflection/Any.h"
#include "Framework/Reflection/Argument.h"
#include "Framework/Reflection/Handle.h"
#include "Framework/Reflection/Method.h"
#include "Framework/Reflection/Property.h"
#include "Framework/Reflection/ReflectionID.h"
#include "Framework/Reflection/ReflectionManager.h"
#include "Framework/Reflection/ReflectionModule.h"
#include "Framework/Reflection/Registration.h"
#include "Framework/Reflection/SubclassOf.h"
#include "Framework/Reflection/Type.h"

#include "Framework/Serialization/AdditionalOperators.h"
#include "Framework/Serialization/Archive.h"
#include "Framework/Serialization/StreamArchive.h"

#include "Application/Application.h"
#include "Application/Keys.h"
#include "Application/Window.h"

#include "AssetSystem/Asset.h"
#include "AssetSystem/AssetID.h"
#include "AssetSystem/AssetRef.h"
#include "AssetSystem/AssetImporter.h"
#include "AssetSystem/AssetStorage.h"
#include "AssetSystem/AssetStorage_Directory.h"
#include "AssetSystem/Importers/AssetImporter_stbi.h"
#include "AssetSystem/Importers/AssetImporter_tinyobj.h"
#include "AssetSystem/AssetSystem.h"

#include "Engine/Engine.h"
#include "Engine/EngineModule.h"
#include "Engine/Project.h"

#include "FileSystem/Paths.h"

#include "Game/GameInstance.h"
#include "Game/World.h"
#include "Game/Entity.h"
//#include "Game/Components/CameraComponent.h"
//#include "Game/Components/DirectionallightComponent.h"
#include "Game/Components/EntityComponent.h"
#include "Game/Components/GeometryComponent.h"
//#include "Game/Components/InstancedStaticMeshComponent.h"
#include "Game/Components/LightComponent.h"
//#include "Game/Components/PointlightComponent.h"
//#include "Game/Components/SkeletalMeshComponent.h"
//#include "Game/Components/SpotlightComponent.h"
#include "Game/Components/StaticMeshComponent.h"
#include "Game/Components/TransformComponent.h"

#include "Material/Texture.h"
#include "Material/Material.h"

#include "Mesh/MeshVertex.h"
#include "Mesh/StaticMesh.h"
#include "Mesh/StaticMeshFactory.h"

#include "Rendering/RHI/DynamicRHI.h"
#include "Rendering/RHI/Enums.h"
#include "Rendering/RHI/Pipeline.h"
#include "Rendering/RHI/RHIBuffer.h"
#include "Rendering/RHI/RHIContext.h"
#include "Rendering/RHI/RHIContextImmediate.h"
#include "Rendering/RHI/RHIImage.h"
#include "Rendering/RHI/RHIObject.h"
#include "Rendering/RHI/Sampler.h"
#include "Rendering/RHI/Shader.h"
#include "Rendering/RHI/Swapchain.h"

#include "Rendering/RenderTarget.h"
#include "Rendering/ShaderCompiler.h"

#include "Threading/AudioThread.h"
#include "Threading/LambdaTask.h"
#include "Threading/RenderThread.h"
#include "Threading/TaskBase.h"
#include "Threading/TaskSystem.h"


using namespace CE;
namespace CardinalEngine = CE;
