#include "Window_Windows.h"

#include <Windows.h>

namespace CE
{
	Window_Windows::Window_Windows(void* NativeHandle) : NativeHandle(NativeHandle)
	{
		const RAWINPUTDEVICE rid = { 0x01, 0x02, 0, (HWND)NativeHandle };
		RegisterRawInputDevices(&rid, 1, sizeof(rid));
	}

	Window_Windows::~Window_Windows()
	{
		const RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_REMOVE, (HWND)NativeHandle };
		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		::DestroyWindow((HWND)NativeHandle);
	}

	void Window_Windows::Close()
	{
		CloseRequested = true;
	}

	void Window_Windows::SetTitle(const Text& Title)
	{
		::SetWindowTextW((HWND)NativeHandle, Title.Data());
	}

	Text Window_Windows::GetTitle() const
	{
		wchar_t wnd_title[1024];
		::GetWindowTextW((HWND)NativeHandle, wnd_title, sizeof(wnd_title));
		return wnd_title;
	}

	Vec2ui Window_Windows::GetSize() const
	{
		RECT Area;
		::GetClientRect((HWND)NativeHandle, &Area);
		return { (uint32_t)Area.right, (uint32_t)Area.bottom };
	}

	void Window_Windows::SetDisplayMode(EWindowDisplayMode Mode)
	{
		switch (Mode)
		{
		default:
		case EWindowDisplayMode::Default:
			::ShowWindow((HWND)NativeHandle, SW_RESTORE);
			break;
		case EWindowDisplayMode::Hidden:
			::ShowWindow((HWND)NativeHandle, SW_HIDE);
			break;
		case EWindowDisplayMode::Minimized:
			::ShowWindow((HWND)NativeHandle, SW_MINIMIZE);
			break;
		case EWindowDisplayMode::Maximized:
			::ShowWindow((HWND)NativeHandle, SW_MAXIMIZE);
			break;
		}
	}

	EWindowDisplayMode Window_Windows::GetDisplayMode() const
	{
		WINDOWPLACEMENT place{};
		place.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement((HWND)NativeHandle, &place);
		if (place.showCmd == SW_HIDE)
		{
			return EWindowDisplayMode::Hidden;
		}
		else if (place.showCmd == SW_MINIMIZE || place.showCmd == SW_SHOWMINIMIZED)
		{
			return EWindowDisplayMode::Minimized;
		}
		else if (place.showCmd == SW_MAXIMIZE)
		{
			return EWindowDisplayMode::Maximized;
		}
		else
		{
			return EWindowDisplayMode::Default;
		}
	}

	void Window_Windows::Focus()
	{
		::BringWindowToTop((HWND)NativeHandle);
		::SetForegroundWindow((HWND)NativeHandle);
		::SetFocus((HWND)NativeHandle);
	}

	void Window_Windows::RequestFocus()
	{
		::FlashWindow((HWND)NativeHandle, TRUE);
	}

	bool Window_Windows::HasFocus() const
	{
		return (HWND)NativeHandle == ::GetActiveWindow();
	}

	void Window_Windows::SetOpacity(float Opacity)
	{
		if (Opacity < 1.f)
		{
			const BYTE alpha = (BYTE)(255 * Opacity);
			DWORD style = GetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE);
			style |= WS_EX_LAYERED;
			SetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE, style);
			SetLayeredWindowAttributes((HWND)NativeHandle, 0, alpha, LWA_ALPHA);
		}
		else
		{
			DWORD style = GetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE);
			style &= ~WS_EX_LAYERED;
			SetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE, style);
		}
	}

	float Window_Windows::GetOpacity() const
	{
		BYTE alpha;
		DWORD flags;

		if ((GetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE) & WS_EX_LAYERED) &&
			GetLayeredWindowAttributes((HWND)NativeHandle, NULL, &alpha, &flags))
		{
			if (flags & LWA_ALPHA)
				return alpha / 255.0f;
		}

		return 1.0f;
	}

	void Window_Windows::SetStyleFlag(EWindowStyleFlag Flag, bool Value)
	{
		DWORD Style = ::GetWindowLongW((HWND)NativeHandle, GWL_STYLE);
		DWORD Exstyle = ::GetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE);

		switch (Flag)
		{
		case EWindowStyleFlag::Decorated:
		{
			if (Value)
			{
				Style |= WS_CAPTION;
			}
			else
			{
				Style &= ~(WS_CAPTION);
			}
			break;
		}
		case EWindowStyleFlag::HasTaskbarIcon:
		{
			if (Value)
			{
				Exstyle |= WS_EX_APPWINDOW;
				Exstyle &= ~(WS_EX_TOOLWINDOW);
			}
			else
			{
				Exstyle |= WS_EX_TOOLWINDOW;
				Exstyle &= ~(WS_EX_APPWINDOW);
			}
			break;
		}
		case EWindowStyleFlag::ResizeableBorder:
		{
			if (Value)
			{
				Style |= WS_THICKFRAME;
			}
			else
			{
				Style &= ~(WS_THICKFRAME);
			}
			break;
		}
		case EWindowStyleFlag::ResizeButton:
		{
			if (Value)
			{
				Style |= WS_MAXIMIZEBOX;
			}
			else
			{
				Style &= ~WS_MAXIMIZEBOX;
			}
			break;
		}
		}

		UpdateWindowStyle(Style, Exstyle);
	}

	bool Window_Windows::GetStyleFlag(EWindowStyleFlag Flag) const
	{
		DWORD Style = ::GetWindowLongW((HWND)NativeHandle, GWL_STYLE);
		DWORD Exstyle = ::GetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE);

		switch (Flag)
		{
		case EWindowStyleFlag::Decorated:
		{
			return Style & WS_CAPTION;
		}
		case EWindowStyleFlag::HasTaskbarIcon:
		{
			return Exstyle & WS_EX_APPWINDOW;
		}
		case EWindowStyleFlag::ResizeableBorder:
		{
			return Style & WS_THICKFRAME;
		}
		case EWindowStyleFlag::ResizeButton:
		{
			return Style & WS_MAXIMIZEBOX;
		}
		}

		return false;
	}

	void Window_Windows::SetCursorMode(ECursorMode Mode)
	{
		if (CursorMode != Mode && Mode == ECursorMode::Disabled)
		{
			DisableCursor(true);
		}
		else if (CursorMode != Mode && Mode != ECursorMode::Disabled)
		{
			EnableCursor(false);
		}

		CursorMode = Mode;
	}

	ECursorMode Window_Windows::GetCursorMode() const
	{
		return CursorMode;
	}

	void Window_Windows::SetCursor(ECursor InCursor)
	{
		Cursor = InCursor;
	}

	ECursor Window_Windows::GetCursor() const
	{
		return Cursor;
	}

	void* Window_Windows::GetNativeHandle() const
	{
		return NativeHandle;
	}

	void Window_Windows::DisableCursor(bool SaveCursorPos)
	{
		UpdateCursorClip(ECursorMode::Disabled);
		CursorDisabled = true;

		if (SaveCursorPos)
		{
			POINT CursorPos;
			::GetCursorPos(&CursorPos);
			RestoreCursorPos.X = CursorPos.x;
			RestoreCursorPos.Y = CursorPos.y;
		}

	}

	void Window_Windows::EnableCursor(bool InRestoreCursorPos)
	{
		UpdateCursorClip(ECursorMode::Default);
		CursorDisabled = false;

		if (InRestoreCursorPos)
		{
			::SetCursorPos(RestoreCursorPos.X, RestoreCursorPos.Y);
		}
	}

	void Window_Windows::UpdateCursorClip(ECursorMode Mode)
	{
		if (Mode == ECursorMode::Disabled)
		{
			RECT CursorClipRect{ 0,0,0,0 };
			::GetClientRect((HWND)NativeHandle, &CursorClipRect);
			::ClientToScreen((HWND)NativeHandle, (POINT*)&CursorClipRect.left);
			::ClientToScreen((HWND)NativeHandle, (POINT*)&CursorClipRect.right);
			::ClipCursor(&CursorClipRect);
		}
		else
		{
			::ClipCursor(NULL);
		}
	}

	void Window_Windows::UpdateCursor()
	{
		LPWSTR WinCursor = IDC_ARROW;
		switch (Cursor)
		{
		case ECursor::None:
			WinCursor = NULL;
			break;
		default:
		case ECursor::Arrow:
			WinCursor = IDC_ARROW;
			break;
		case ECursor::TextInput:
			WinCursor = IDC_IBEAM;
			break;
		case ECursor::Resize_All:
			WinCursor = IDC_SIZEALL;
			break;
		case ECursor::Resize_EW:
			WinCursor = IDC_SIZEWE;
			break;
		case ECursor::Resize_NS:
			WinCursor = IDC_SIZENS;
			break;
		case ECursor::Resize_NESW:
			WinCursor = IDC_SIZENESW;
			break;
		case ECursor::Resize_NWSE:
			WinCursor = IDC_SIZENWSE;
			break;
		case ECursor::Hand:
			WinCursor = IDC_HAND;
			break;
		case ECursor::NotAllowed:
			WinCursor = IDC_NO;
			break;
		}
		::SetCursor(::LoadCursor(NULL, WinCursor));
	}

	void Window_Windows::UpdateWindowStyle(unsigned long Style, unsigned long ExStyle)
	{
		RECT rect;
		::GetClientRect((HWND)NativeHandle, &rect);
		::AdjustWindowRectEx(&rect, Style, FALSE, ExStyle);
		::ClientToScreen((HWND)NativeHandle, (POINT*)&rect.left);
		::ClientToScreen((HWND)NativeHandle, (POINT*)&rect.right);

		::SetWindowLongW((HWND)NativeHandle, GWL_STYLE, Style);
		::SetWindowLongW((HWND)NativeHandle, GWL_EXSTYLE, ExStyle);

		//::SetWindowPos((HWND)NativeHandle, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
		::SetWindowPos((HWND)NativeHandle, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
	}
}
