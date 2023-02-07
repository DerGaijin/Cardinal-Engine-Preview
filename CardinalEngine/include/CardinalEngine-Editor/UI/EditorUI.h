#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "EditorWindow.h"
#include "Windows/AssetBrowser.h"
#include "Windows/EditorLog.h"
#include "Windows/MainEditor.h"
#include "Windows/ProfilerDisplay.h"
#include "Windows/StatisticsDisplay.h"

namespace CE
{
	class CE_API EditorUI
	{
	public:
		EditorUI();

		void Tick(float DeltaTime);
	
		void Draw(float DeltaTime);


	private:
		void DrawMainMenuBar();
		void DrawDockSpace();

	public:
		AssetBrowser AssetBrowser;
		EditorLog EditorLog;
		MainEditor MainEditor;
		ProfilerDisplay ProfilerDisplay;
		StatisticsDisplay StatisticsDisplay;

		Array<SharedPtr<EditorWindow>> Windows;

	private:
		bool m_ShowImGuiDemo = false;
	};
}
