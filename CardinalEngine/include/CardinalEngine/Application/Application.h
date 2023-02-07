#pragma once
#include "../Framework/Core/Headers.h"
#include "Monitor.h"
#include "Window.h"
#include "Keys.h"

namespace CE
{
	class CE_API Application : public Singleton
	{
	public:
		static Application* Create();

		virtual bool Initialize() = 0;

		virtual void Tick(float DeltaTime) = 0;

		virtual void Terminate() = 0;

	public:
		// Get all connected monitors
		virtual Array<Monitor> GetMonitors() const = 0;

		// Create a platform specific window
		virtual SharedPtr<Window> CreateWindow(const Text& Title, const Vec2ui& Size, EWindowDisplayMode DisplayMode = EWindowDisplayMode::Default) = 0;

		// Create a platform specific window at position
		virtual SharedPtr<Window> CreateWindow(const Text& Title, const Vec2ui& Size, const Vec2ui& Pos, EWindowDisplayMode DisplayMode = EWindowDisplayMode::Default) = 0;

		// Returns the platform window that is in the foreground and was created by this application
		virtual Window* GetForegroundWindow() const = 0;

		// Returns the platform window that is active and was created by this application
		virtual Window* GetActiveWindow() const = 0;

		// Returns the platform window that is at the point and was created by this application
		virtual Window* GetWindowFromPoint(const Vec2f& ScreenPosition) const = 0;

		// Returns the monitor that the Window is in and was created by this application
		virtual Monitor GetMonitorFromWindow(Window* Window) const = 0;

	public:
		// Returns true if the input is pressed
		virtual bool IsInputPressed(EInput Input) const = 0;

		// Returns the platform specific input name
		virtual String NameOfInput(EInput Input) const = 0;

		// Returns the current cursor position, if Origin is valid it returns the position relative to the Origin
		virtual Vec2<long> GetCursorPosition(Window* Origin = nullptr) const = 0;

		// Sets the current cursor position, if Origin is valid it sets the position relative to the Origin
		virtual void SetCursorPosition(const Vec2<long>& Pos, Window* Origin = nullptr) const = 0;

	};
}
