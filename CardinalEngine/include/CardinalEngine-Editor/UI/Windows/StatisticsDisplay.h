#pragma once
#include "../../../CardinalEngine/Framework/Core/Headers.h"
#include "../EditorWindow.h"

namespace CE
{
	class Method;
	class Property;

	class CE_API StatisticsDisplay : public EditorWindow
	{
	public:
		virtual void Draw(float DeltaTime) override;

		virtual bool RemoveOnClosed() override
		{
			return false;
		}

	private:
		void Draw_General(float DeltaTime);
		void Draw_Modules(float DeltaTime);
		void Draw_Reflection(float DeltaTime);
		void Draw_Paths(float DeltaTime);
		void Draw_Rendering(float DeltaTime);
		void Draw_TaskSystem(float DeltaTime);
		void Draw_AssetSystem(float DeltaTime);
		void Draw_GameInstances(float DeltaTime);

		void Draw_Method(const Method& Method, const String& AlternativeName = "");
		void Draw_Property(const Property& Property);

	private:
		bool ShowCategory[6] = { false, false, true, true, false, false };

	};
}
