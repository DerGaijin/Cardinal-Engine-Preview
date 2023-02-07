#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../ImGuiInterface/Headers.h"

namespace CE
{
	class CE_API EditorWindow : public Singleton
	{
	public:
		virtual void Tick(float DeltaTime) {}
		
		virtual void Draw(float DeltaTime) {}
	
		virtual bool RemoveOnClosed() 
		{
			return true; 
		}

	public:
		bool IsOpen = true;
	};
}
