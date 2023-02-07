#pragma once
#include "../Framework/Core/Headers.h"
#include "../Framework/ModuleSystem/Module.h"

namespace CE
{
	class CE_API EngineModule : public Module
	{
	public:
		virtual String Name() const override;

		virtual VersionNumber Version() const override;

	protected:
		virtual void Register() override;

		virtual void Unregister() override;

		virtual void RegisterReflection(ReflectionRegistration& Register) override;

		virtual bool IsCompatibleWith(Module& Module) override;

	};
}
