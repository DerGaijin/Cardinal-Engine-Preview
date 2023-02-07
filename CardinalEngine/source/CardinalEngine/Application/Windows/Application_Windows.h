#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Application/Application.h"

namespace CE
{
	class Application_Windows : public Application
	{
	public:
		virtual bool Initialize() override;

		virtual void Tick(float DeltaTime) override;

		virtual void Terminate() override;

	public:
		virtual Array<Monitor> GetMonitors() const override;

		virtual SharedPtr<Window> CreateWindow(const Text& Title, const Vec2ui& Size, EWindowDisplayMode DisplayMode = EWindowDisplayMode::Default) override;

		virtual SharedPtr<Window> CreateWindow(const Text& Title, const Vec2ui& Size, const Vec2ui& Pos, EWindowDisplayMode DisplayMode = EWindowDisplayMode::Default) override;

		virtual Window* GetForegroundWindow() const override;

		virtual Window* GetActiveWindow() const override;

		virtual Window* GetWindowFromPoint(const Vec2f& ScreenPosition) const override;
		
		virtual Monitor GetMonitorFromWindow(Window* Window) const override;

	public:
		virtual bool IsInputPressed(EInput Input) const override;

		virtual String NameOfInput(EInput Input) const override;

		virtual Vec2<long> GetCursorPosition(Window* Origin = nullptr) const override;

		virtual void SetCursorPosition(const Vec2<long>& Pos, Window* Origin = nullptr) const override;

	public:
		static int InputToScancode(EInput Input);
		static EInput ScancodeToInput(int Scancode);
		static int InputToVirtualKey(EInput Input);

	private:
		Text m_WindowClassName;

		Map<EInput, float> m_InputValues;
	};
}
