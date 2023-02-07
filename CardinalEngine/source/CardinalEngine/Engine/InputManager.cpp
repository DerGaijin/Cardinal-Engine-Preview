#include "CardinalEngine/Engine/InputManager.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Game/GameInstance.h"
#include "CardinalEngine/Application/Application.h"

#if CE_EDITOR
#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"
#include "CardinalEngine-Editor/ImGuiInterface/ImGuiInterface.h"
#endif

namespace CE
{
	bool InputManager::IsInputPressed(EInput Input) const
	{
		return m_PressedInputs.count(Input) > 0;
	}

	float InputManager::GetInputValue(EInput Input) const
	{
		auto Found = m_InputValues.find(Input);
		if (Found != m_InputValues.end())
		{
			return Found->second;
		}

		return IsInputPressed(Input) ? 1.0f : 0.0f;
	}

	void InputManager::ProcessInput(Window* Window, EInput Input, bool Pressed, EInputModifier Modifiers)
	{
		InputEvent_Button Event(Input, Pressed, Modifiers);
		DispatchEvent(Event);

		if (Pressed)
		{
			m_PressedInputs.emplace(Input);
		}
		else
		{
			m_PressedInputs.erase(Input);
		}

		for (auto& ActionEvent : m_ActionEvents)
		{
			if (Pressed)
			{
				for (auto& Key : ActionEvent.second.m_Keys)
				{
					if (Key.Input == Input)
					{
						if (Key.Mod & EInputModifier::Shift && !(Modifiers & EInputModifier::Shift))
						{
							continue;
						}
						if (Key.Mod & EInputModifier::Control && !(Modifiers & EInputModifier::Control))
						{
							continue;
						}
						if (Key.Mod & EInputModifier::Alt && !(Modifiers & EInputModifier::Alt))
						{
							continue;
						}
						if (Key.Mod & EInputModifier::Super && !(Modifiers & EInputModifier::Super))
						{
							continue;
						}
						if (Key.Mod & EInputModifier::CapsLock && !(Modifiers & EInputModifier::CapsLock))
						{
							continue;
						}
						if (Key.Mod & EInputModifier::NumLock && !(Modifiers & EInputModifier::NumLock))
						{
							continue;
						}

						ActionEvent.second.m_Input = Input;
						ActionEvent.second.m_Modifier = Modifiers;
						ActionEvent.second.m_Pressed = true;
						DispatchEvent(ActionEvent.second);
					}
				}
			}
			else if (ActionEvent.second.m_Input == Input)
			{
				ActionEvent.second.m_Input = EInput::None;
				ActionEvent.second.m_Modifier = Modifiers;
				ActionEvent.second.m_Pressed = false;
				DispatchEvent(ActionEvent.second);
			}
		}

#if CE_EDITOR
		if (Input >= EInput::Keyboard_A && Input <= EInput::Keyboard_Up)
		{
			EngineEditor::Get().ImGuiInterface.OnKeyboardKey(Window, Input, Pressed, Modifiers);
		}
		else if (Input >= EInput::Mouse_Left && Input <= EInput::Mouse_8)
		{
			EngineEditor::Get().ImGuiInterface.OnMouseButton(Window, Input, Pressed, Modifiers);
		}
#endif
	}

	void InputManager::ProcessInput(Window* Window, unsigned char Char)
	{
		InputEvent_Char Event(Char);
		DispatchEvent(Event);

#if CE_EDITOR
		EngineEditor::Get().ImGuiInterface.OnChar(Window, Char);
#endif
	}

	void InputManager::ProcessInput(Window* Window, EInput Input, float Value, EInputModifier Modifiers)
	{
		auto Found = m_InputValues.find(Input);
		if (Found != m_InputValues.end())
		{
			Found->second += Value;
		}
		else
		{
			m_InputValues[Input] = Value;
		}

		if (Input == EInput::Mouse_Wheel)
		{
			int Count = Value < 0.0f ? -((int)Value) : (int)Value;
			for (size_t i = 0; i < Count; i++)
			{
				ProcessInput(Window, Value > 0.0f ? EInput::Mouse_WheelUp : EInput::Mouse_WheelDown, true, Modifiers);
				ProcessInput(Window, Value > 0.0f ? EInput::Mouse_WheelUp : EInput::Mouse_WheelDown, false, Modifiers);
			}

#if CE_EDITOR
			EngineEditor::Get().ImGuiInterface.OnMouseScroll(Window, Value, 0.0f);
#endif
		}
	}

	void InputManager::ClearAxisValues()
	{
		m_InputValues.clear();
	}

	void InputManager::ReleaseButtons()
	{
		for (auto& Input : m_PressedInputs)
		{
			for (auto& ActionEvent : m_ActionEvents)
			{
				if (ActionEvent.second.m_Input == Input)
				{
					ActionEvent.second.m_Input = EInput::None;
					ActionEvent.second.m_Modifier = EInputModifier::None;
					ActionEvent.second.m_Pressed = false;
					DispatchEvent(ActionEvent.second);
				}
			}
		}

		m_PressedInputs.clear();
	}

	void InputManager::UpdateAxisValues()
	{
		for (auto& AxisEvent : m_AxisEvents)
		{
			AxisEvent.second.m_Value = 0.0f;
			for (auto& Input : AxisEvent.second.m_Axis)
			{
				AxisEvent.second.m_Value += Input.second * GetInputValue(Input.first);
			}
			DispatchEvent(AxisEvent.second);
		}
	}

	InputEvent_Action& InputManager::AddActionEvent(const String& Name)
	{
		auto Found = m_ActionEvents.find(Name);
		if (Found != m_ActionEvents.end())
		{
			return Found->second;
		}

		return m_ActionEvents.emplace(Name, InputEvent_Action(Name)).first->second;
	}

	InputEvent_Action& InputManager::GetActionEvent(const String& Name)
	{
		auto Found = m_ActionEvents.find(Name);
		if (Found != m_ActionEvents.end())
		{
			return Found->second;
		}

		static InputEvent_Action Empty("");
		return Empty;
	}

	bool InputManager::HasActionEvent(const String& Name) const
	{
		auto Found = m_ActionEvents.find(Name);
		return Found != m_ActionEvents.end();
	}

	bool InputManager::RemoveActionEvent(const String& Name)
	{
		return m_ActionEvents.erase(Name) > 0;
	}

	InputEvent_Axis& InputManager::AddAxisEvent(const String& Name)
	{
		auto Found = m_AxisEvents.find(Name);
		if (Found != m_AxisEvents.end())
		{
			return Found->second;
		}

		return m_AxisEvents.emplace(Name, InputEvent_Axis(Name)).first->second;
	}

	InputEvent_Axis& InputManager::GetAxisEvent(const String& Name)
	{
		auto Found = m_AxisEvents.find(Name);
		if (Found != m_AxisEvents.end())
		{
			return Found->second;
		}

		static InputEvent_Axis Empty("");
		return Empty;
	}

	bool InputManager::HasAxisEvent(const String& Name) const
	{
		auto Found = m_AxisEvents.find(Name);
		return Found != m_AxisEvents.end();
	}

	bool InputManager::RemoveAxisEvent(const String& Name)
	{
		return m_AxisEvents.erase(Name) > 0;
	}

	void InputManager::DispatchEvent(const InputEvent& Event)
	{
		for (auto& GI : Engine::Get().GameInstances())
		{
			if (GI->ProcessInputEvent(Event))
			{
				break;
			}
		}
	}

	void InputEvent_Action::Add(EInput Input, EInputModifier Modifier /*= EKeyModifier::None*/)
	{
		InputMod It;
		It.Input = Input;
		It.Mod = Modifier;

		m_Keys.emplace(It);
	}

	void InputEvent_Action::Remove(EInput Input, EInputModifier Modifier /*= EKeyModifier::None*/)
	{
		InputMod It;
		It.Input = Input;
		It.Mod = Modifier;

		m_Keys.erase(It);
	}

	bool InputEvent_Action::Has(EInput Input, EInputModifier Modifier /*= EKeyModifier::None*/) const
	{
		InputMod It;
		It.Input = Input;
		It.Mod = Modifier;

		return m_Keys.find(It) != m_Keys.end();
	}

	void InputEvent_Axis::Set(EInput Input, float Scale)
	{
		m_Axis[Input] = Scale;
	}

	float InputEvent_Axis::Get(EInput Input) const
	{
		auto Found = m_Axis.find(Input);
		if (Found != m_Axis.end())
		{
			return Found->second;
		}
		return 0.0f;
	}
}
