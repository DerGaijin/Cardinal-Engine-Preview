#pragma once
#include "../../Framework/Core/Headers.h"
#include "../../Framework/Delegates/DelegateSingle.h"
#include "EntityComponent.h"
#include "../../Framework/Delegates/DelegateMulti.h"
#include "../../Application/Keys.h"

namespace CE
{
	class InputEvent;
	class InputEvent_Action;
	class InputEvent_Axis;

	class CE_API InputComponent : public EntityComponent
	{
		friend class GameInstance;
	public:
		InputComponent() = default;

		InputComponent(const InputComponent&) = delete;

		InputComponent& operator=(const InputComponent&) = delete;

		void SetEnabled(bool IsEnabled);

		bool IsEnabled() const
		{
			return m_IsEnabled;
		}
	
	public:
		bool IsInputPressed(EInput Input) const;

		float GetInputValue(EInput Input) const;
		
		bool IsActionPressed(const String& ActionName) const;

		float GetAxisValue(const String& AxisName) const;

	private:
		bool ProcessInputEvent(const InputEvent& Event);

	public:
		DelegateMulti<const InputEvent_Action&> OnActionEvent;

		DelegateMulti<const InputEvent_Axis&> OnAxisEvent;

	private:
		bool m_IsEnabled = true;

	};
}
