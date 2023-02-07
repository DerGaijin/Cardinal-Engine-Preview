#pragma once
#include "CardinalEngine/CardinalEngine.h"
#if CE_EDITOR
#include "CardinalEngine-Editor/CardinalEngine-Editor.h"
#endif

class DevelopmentProject_Module : public Module
{
public:
	virtual String Name() const override;

	virtual VersionNumber Version() const override;
};

class DemoScene
{
public:
	void Initialize();

	void Tick(float DeltaTime);

	void Terminate();

public:
	SharedPtr<World> DemoWorld;

	SharedPtr<StaticMeshComponent> RotMovCube;

	bool BounceSphereBackwards = true;
	SharedPtr<StaticMeshComponent> BounceSphere;

	SharedPtr<StaticMeshComponent> LookAtCube;
};

class DevelopmentProject_Project : public Project
{
public:
	virtual Module& GetModule() const override;

protected:
	virtual bool Initialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Terminate() override;

public:
	DemoScene Demo;
};
