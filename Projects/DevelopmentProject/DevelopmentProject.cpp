#include "DevelopmentProject.h"

String DevelopmentProject_Module::Name() const
{
	return "Development Project";
}

VersionNumber DevelopmentProject_Module::Version() const
{
	return VersionNumber(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
}

Module& DevelopmentProject_Project::GetModule() const
{
	static DevelopmentProject_Module Inst;
	return Inst;
}

bool DevelopmentProject_Project::Initialize()
{
	Demo.Initialize();

	return true;
}

void DevelopmentProject_Project::Tick(float DeltaTime)
{
	Demo.Tick(DeltaTime);
}

void DevelopmentProject_Project::Terminate()
{
	Demo.Terminate();
}


void DemoScene::Initialize()
{
	DemoWorld = SharedPtr<World>(new World());
	DemoWorld->EditorName = "Demo World";

#if CE_EDITOR
	EditorGameInstance::Get()->AddWorld(DemoWorld);
#else
#error No Game Instance
#endif

	AssetRef<Material> TestMaterial = Engine::Get().AssetSystem.Get<Material>(AssetID("CardinalEngine-Editor", "Assets/Materials/Material 1"));

	// Add Floor
	{
		SharedPtr<StaticMeshComponent> FloorMeshComp = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		FloorMeshComp->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(5.0f, 5.0f, 0.05f)));
		SharedPtr<Entity> FloorMeshEntity = SharedPtr<Entity>(new Entity(FloorMeshComp));
		FloorMeshEntity->EditorName = "Floor";
		DemoWorld->AddEntity(FloorMeshEntity);
	}

	// World Coordinate System
	{		
		SharedPtr<TransformComponent> WorldCoordComp = SharedPtr<TransformComponent>(new TransformComponent());
		WorldCoordComp->SetRelativeLocation(Vec3f(-2.0f, 2.0f, 0.6f));
		SharedPtr<Entity> WorldCoordEntity = SharedPtr<Entity>(new Entity(WorldCoordComp));
		WorldCoordEntity->EditorName = "World Coordinates";
		DemoWorld->AddEntity(WorldCoordEntity);

		SharedPtr<StaticMeshComponent> CompX = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompX->EditorName = "Component X";
		CompX->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(0.5f, 0.1f, 0.1f), Color::Red));
		CompX->SetRelativeLocation(Vec3f(0.25f, 0.0f, 0.0f));
		WorldCoordComp->AddComponent(CompX);

		SharedPtr<StaticMeshComponent> CompY = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompY->EditorName = "Component Y";
		CompY->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(0.1f, 0.5f, 0.1f), Color::Green));
		CompY->SetRelativeLocation(Vec3f(0.0f, 0.25f, 0.0f));
		WorldCoordComp->AddComponent(CompY);
		
		SharedPtr<StaticMeshComponent> CompZ = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompZ->EditorName = "Component Z";
		CompZ->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(0.1f, 0.1f, 0.5f), Color::Blue));
		CompZ->SetRelativeLocation(Vec3f(0.0f, 0.0f, 0.25f));
		WorldCoordComp->AddComponent(CompZ);
	}

	// Add Rotating/Moving Cube
	{
		RotMovCube = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		RotMovCube->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(0.5f), Color::MediumVioletRed));
		RotMovCube->SetMaterial(TestMaterial);
		RotMovCube->SetRelativeLocation(Vec3f(-1.0f, 2.0f, 0.6f));
		SharedPtr<Entity> CubeEntity = SharedPtr<Entity>(new Entity(RotMovCube));
		CubeEntity->EditorName = "Rotating/Moving Cube";
		DemoWorld->AddEntity(CubeEntity);
	}

	// Bouncing Sphere
	{
		BounceSphere = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		BounceSphere->SetStaticMesh(StaticMeshFactory::UVSphere(0.3f, 30, 30, Color::BlueViolet));
		BounceSphere->SetMaterial(TestMaterial);
		BounceSphere->SetRelativeLocation(Vec3f(0.0f, 2.0f, 0.6f));
		SharedPtr<Entity> SphereEntity = SharedPtr<Entity>(new Entity(BounceSphere));
		SphereEntity->EditorName = "Bounce Sphere";
		DemoWorld->AddEntity(SphereEntity);
	}

	// Bouncing Sphere Look At
	{
		SharedPtr<StaticMeshComponent> CompX = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompX->EditorName = "Component X";
		CompX->SetStaticMesh(StaticMeshFactory::Cube(0.2f, Color::Red));
		CompX->SetRelativeLocation(Vec3f(1.0f, 0.0f, 0.0f));
		SharedPtr<StaticMeshComponent> CompY = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompY->EditorName = "Component Y";
		CompY->SetStaticMesh(StaticMeshFactory::Cube(0.2f, Color::Green));
		CompY->SetRelativeLocation(Vec3f(0.0f, 1.0f, 0.0f));
		SharedPtr<StaticMeshComponent> CompZ = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		CompZ->EditorName = "Component Z";
		CompZ->SetStaticMesh(StaticMeshFactory::Cube(0.2f, Color::Blue));
		CompZ->SetRelativeLocation(Vec3f(0.0f, 0.0f, 1.0f));

		LookAtCube = SharedPtr<StaticMeshComponent>(new StaticMeshComponent());
		LookAtCube->SetStaticMesh(StaticMeshFactory::Cube(Vec3f(0.3, 0.3f, 0.3f), Color::Orange));
		LookAtCube->SetRelativeLocation(Vec3f(2.0f, 2.0f, 0.6f));
		SharedPtr<Entity> LookAtCubeEntity = SharedPtr<Entity>(new Entity(LookAtCube));
		LookAtCubeEntity->EditorName = "LookAt Cube";
		DemoWorld->AddEntity(LookAtCubeEntity);

		LookAtCube->AddComponent(CompX);
		LookAtCube->AddComponent(CompY);
		LookAtCube->AddComponent(CompZ);
	}
}

void DemoScene::Tick(float DeltaTime)
{
	// Rotate/Move Cube
	{
		static const Vec3f RotateSpeed = Vec3f(20.0f, 25.0f, 30.0f);
		Vec3f Rotation = RotMovCube->GetRelativeRotation();
		Rotation.X += RotateSpeed.X * DeltaTime;
		while (Rotation.X >= 360.0f)
		{
			Rotation.X -= 360.0f;
		}
		Rotation.Y += RotateSpeed.Y * DeltaTime;
		while (Rotation.Y >= 360.0f)
		{
			Rotation.Y -= 360.0f;
		}
		Rotation.Z += RotateSpeed.Z * DeltaTime;
		while (Rotation.Z >= 360.0f)
		{
			Rotation.Z -= 360.0f;
		}
		RotMovCube->SetRelativeRotation(Rotation);
	}


	// Bounce Sphere
	{
		static const float MoveSpeed = 0.2f;
		Vec3f Location = BounceSphere->GetRelativeLocation();
		Location.Z += (BounceSphereBackwards ? -MoveSpeed : MoveSpeed) * DeltaTime;
		if (Location.Z <= 0.2f)
		{
			BounceSphereBackwards = false;
		}
		if (Location.Z >= 1.0f)
		{
			BounceSphereBackwards = true;
		}
		BounceSphere->SetRelativeLocation(Location);
	}


	// LookAt Cube
	{
		Vec3f SourceLocation = LookAtCube->GetWorldLocation();
		Vec3f TargetLocation = BounceSphere->GetWorldLocation();
		Quaternion Rotation = Quaternion::LookAtSafe(SourceLocation, TargetLocation, World::UpVector);
		LookAtCube->SetRelativeRotation(Math::Degrees(Rotation.ToEuler()));
	}
}

void DemoScene::Terminate()
{
	if (DemoWorld != nullptr && DemoWorld->GameInstance() != nullptr)
	{
		DemoWorld->GameInstance()->RemoveWorld(DemoWorld);
	}
	DemoWorld.Reset();
}



CE_SET_PROJECT(DevelopmentProject_Project);
#include "CardinalEngine/Entrypoint.h"
