#include "CardinalEngine/Game/Components/InputComponent.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Engine/InputManager.h"

namespace CE
{
	void InputComponent::SetEnabled(bool IsEnabled)
	{
		m_IsEnabled = IsEnabled;
	}

	bool InputComponent::IsInputPressed(EInput Input) const
	{
		return m_IsEnabled ? Engine::Get().InputManager.IsInputPressed(Input) : false;
	}

	float InputComponent::GetInputValue(EInput Input) const
	{
		return m_IsEnabled ? Engine::Get().InputManager.GetInputValue(Input) : 0.0f;
	}

	bool InputComponent::IsActionPressed(const String& ActionName) const
	{
		return m_IsEnabled ? Engine::Get().InputManager.GetActionEvent(ActionName).IsPressed() : false;
	}

	float InputComponent::GetAxisValue(const String& AxisName) const
	{
		return m_IsEnabled ? Engine::Get().InputManager.GetAxisEvent(AxisName).GetValue() : 0.0f;
	}

	bool InputComponent::ProcessInputEvent(const InputEvent& Event)
	{
		const InputEvent_Action* IEAction = dynamic_cast<const InputEvent_Action*>(&Event);
		if (IEAction != nullptr)
		{
			OnActionEvent.Execute(*IEAction);
		}

		const InputEvent_Axis* IEAxis = dynamic_cast<const InputEvent_Axis*>(&Event);
		if (IEAxis != nullptr)
		{
			OnAxisEvent.Execute(*IEAxis);
		}

		return false;
	}
}
