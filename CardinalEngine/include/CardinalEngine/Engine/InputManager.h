#pragma once
#include "../Framework/Core/Headers.h"
#include "../Application/Keys.h"

namespace CE
{
	class Window;

	class CE_API InputEvent
	{
	public:
		InputEvent(const String& Name) : Name(Name) {}
		virtual ~InputEvent() = default;
		const String Name;
	};

	
	class CE_API InputEvent_Button : public InputEvent
	{
	public:
		InputEvent_Button(EInput Button, bool Pressed, EInputModifier Mods) : InputEvent(""), Button(Button), Pressed(Pressed), Modifiers(Mods) {}
		const EInput Button = EInput::None;
		const bool Pressed = false;
		const EInputModifier Modifiers = EInputModifier::None;
	};


	class CE_API InputEvent_Char : public InputEvent
	{
	public:
		InputEvent_Char(unsigned char Char) : InputEvent(""), Char(Char) {}
		const unsigned short Char = 0;
	};


	class CE_API InputEvent_Action : public InputEvent
	{
		friend class InputManager;
	public:
		InputEvent_Action(const String& Name) : InputEvent(Name) {}

		// Adds a event trigger
		void Add(EInput Input, EInputModifier Modifier = EInputModifier::None);
		
		// Removes a event trigger
		void Remove(EInput Input, EInputModifier Modifier = EInputModifier::None);

		// Returns true if it has a event trigger
		bool Has(EInput Input, EInputModifier Modifier = EInputModifier::None) const;

	public:
		// Returns the trigger that fired this event
		EInput GetButton() const
		{
			return m_Input;
		}

		// Returns the modifier that fired this event
		EInputModifier GetModifiers() const
		{
			return m_Modifier;
		}

		// Returns true if the event is active
		bool IsPressed() const
		{
			return m_Pressed;
		}

	private:
		struct InputMod 
		{
			EInput Input = EInput::None;
			EInputModifier Mod = EInputModifier::None;

			bool operator<(const InputMod& Other) const
			{
				if (Input == Other.Input)
				{
					return Mod < Other.Mod;
				}
				return Input < Other.Input;
			}
		};
		CE::Set<InputMod> m_Keys;

		EInput m_Input = EInput::None;
		EInputModifier m_Modifier = EInputModifier::None;
		bool m_Pressed = false;
	};


	class CE_API InputEvent_Axis : public InputEvent
	{
		friend class InputManager;
	public:
		InputEvent_Axis(const String& Name) : InputEvent(Name) {}

		// Set a trigger with scale for the Event
		void Set(EInput Input, float Scale);

		// Gets the scale for the trigger
		float Get(EInput Input) const;

	public:
		// Returns the current value of the Axis
		float GetValue() const
		{
			return m_Value;
		}

	private:
		Map<EInput, float> m_Axis;

		float m_Value = 0.0f;
	};


	class CE_API InputManager : public Singleton
	{
	public:
		// Returns true if the Input is pressed
		bool IsInputPressed(EInput Input) const;

		// Returns the axis value of the Input
		float GetInputValue(EInput Input) const;

		// Processes a Input trigger
		void ProcessInput(Window* Window, EInput Input, bool Pressed, EInputModifier Modifiers);

		// Processes a char trigger
		void ProcessInput(Window* Window, unsigned char Char);
		
		// Processes a axis trigger
		void ProcessInput(Window* Window, EInput Input, float Value, EInputModifier Modifiers);
		
		// Clears all axis values to default
		void ClearAxisValues();

		// Releases all pressed buttons
		void ReleaseButtons();

		// Updates all axis values
		void UpdateAxisValues();


		// Adds a action event
		InputEvent_Action& AddActionEvent(const String& Name);

		// Gets the action event by name
		InputEvent_Action& GetActionEvent(const String& Name);

		// Returns true if a action event is registered with the name
		bool HasActionEvent(const String& Name) const;

		// Removes a action event by name
		bool RemoveActionEvent(const String& Name);


		// Adds a axis event
		InputEvent_Axis& AddAxisEvent(const String& Name);

		// Gets the axis event by name
		InputEvent_Axis& GetAxisEvent(const String& Name);

		// Returns true if a axis event is registered with the name
		bool HasAxisEvent(const String& Name) const;

		// Removes a axis event by name
		bool RemoveAxisEvent(const String& Name);

	private:
		void DispatchEvent(const InputEvent& Event);

	private:
		Map<String, InputEvent_Action> m_ActionEvents;
		Map<String, InputEvent_Axis> m_AxisEvents;

		Map<EInput, float> m_InputValues;
		Set<EInput> m_PressedInputs;
	};
}
