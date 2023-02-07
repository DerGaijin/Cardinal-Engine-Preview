R"-(#include "$PROJECT_NAME$.h"
#if CE_EDITOR
#include "CardinalEngine-Editor/CardinalEngine-Editor.h"
#endif

$PROJECT_NAME$_Module& $PROJECT_NAME$_Module::Get()
{
	static $PROJECT_NAME$_Module Inst;
	return Inst;
}

String $PROJECT_NAME$_Module::Name() const
{
	return "$PROJECT_NAME$";
}

VersionNumber $PROJECT_NAME$_Module::Version() const
{
	return VersionNumber(1, 0, 0);
}

Module& $PROJECT_NAME$_Project::GetModule() const
{
	return $PROJECT_NAME$_Module::Get();
}

bool $PROJECT_NAME$_Project::Initialize()
{
	return true;
}

void $PROJECT_NAME$_Project::Tick(float DeltaTime)
{

}

void $PROJECT_NAME$_Project::Terminate()
{

}

#include "CardinalEngine/Entrypoint.h"
CE_SET_PROJECT($PROJECT_NAME$_Project)
)-"
