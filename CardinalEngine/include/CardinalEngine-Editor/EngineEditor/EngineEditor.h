#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "EngineEditorModule.h"
#include "../ImGuiInterface/ImGuiInterface.h"

namespace CE
{
	class CE_API EngineEditor : public Singleton
	{
	public:
		EngineEditor();
		
		void Initialize();

		void BeginFrame(float DeltaTime);

		void Tick(float DeltaTime);

		void EndFrame();

		void Terminate();

		~EngineEditor();
	
	public:
		static EngineEditor& Get();


	public:
		EngineEditorModule Module;

		ImGuiInterface ImGuiInterface;

	private:
		static EngineEditor* s_EditorInstance;
	};
}
