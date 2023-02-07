#pragma once
#include "../Framework/Core/Headers.h"

namespace CE
{
	class Monitor;
	class Swapchain;
	class DynamicRHI;

	enum class EWindowDisplayMode
	{
		Default,
		Hidden,
		Minimized,
		Maximized,
	};

	enum class ECursorMode
	{
		Default,
		Hidden,
		Disabled,
	};

	enum class ECursor
	{
		None,
		Arrow,
		TextInput,
		Resize_All,
		Resize_EW,
		Resize_NS,
		Resize_NESW,
		Resize_NWSE,
		Hand,
		NotAllowed,
	};

	enum class EWindowStyleFlag
	{
		Decorated = 0,
		HasTaskbarIcon,
		ResizeableBorder,
		ResizeButton,
	};

	class CE_API Window : public Singleton
	{
		friend class DynamicRHI;
	public:

		// Mark this window as closed
		virtual void Close() = 0;

		// Returns true if Close() was called and needs to be destroyed
		virtual bool IsCloseRequested() const = 0;

		// Sets the window Title
		virtual void SetTitle(const Text& Title) = 0;

		// Returns the window Title
		virtual Text GetTitle() const = 0;

		// Returns the window size
		virtual Vec2ui GetSize() const = 0;

		// Sets the window display mode
		virtual void SetDisplayMode(EWindowDisplayMode Mode) = 0;

		// Returns the Display Mode
		virtual EWindowDisplayMode GetDisplayMode() const = 0;

		// Focuses the window and brings it to foreground
		virtual void Focus() = 0;

		// Request the users attention
		virtual void RequestFocus() = 0;
		
		// Returns true if the window has the users focus
		virtual bool HasFocus() const = 0;

		// Sets the opacity of the window
		virtual void SetOpacity(float Opacity) = 0;

		// Returns the opacity of the window
		virtual float GetOpacity() const = 0;

		// Sets a window style flag value
		virtual void SetStyleFlag(EWindowStyleFlag Flag, bool Value) = 0;

		// Returns the value of the window style flag
		virtual bool GetStyleFlag(EWindowStyleFlag Flag) const = 0;
		
		// Set the cursor mode
		virtual void SetCursorMode(ECursorMode Mode) = 0;

		// Returns the cursor mode
		virtual ECursorMode GetCursorMode() const = 0;

		// Set the current cursor type
		virtual void SetCursor(ECursor Cursor) = 0;

		// Get the current cursor type
		virtual ECursor GetCursor() const = 0;

		// Returns the platform specific window handle
		virtual void* GetNativeHandle() const = 0;

	public:
		// Returns the Swapchain
		SharedPtr<Swapchain> GetSwapchain() const
		{
			return m_Swapchain;
		}

	protected:
		SharedPtr<Swapchain> m_Swapchain;
	};
}
