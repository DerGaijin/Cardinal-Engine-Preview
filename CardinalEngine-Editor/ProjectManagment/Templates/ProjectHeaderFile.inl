R"-(#pragma once
#include "CardinalEngine/CardinalEngine.h"

class $PROJECT_NAME$_Module : public Module
{
	$PROJECT_NAME$_Module() = default;
public:
	static $PROJECT_NAME$_Module& Get();

public:
	virtual String Name() const override;

	virtual VersionNumber Version() const override;
};



class $PROJECT_NAME$_Project : public Project
{
public:
	virtual Module& GetModule() const override;

protected:
	virtual bool Initialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Terminate() override;
};
)-"
