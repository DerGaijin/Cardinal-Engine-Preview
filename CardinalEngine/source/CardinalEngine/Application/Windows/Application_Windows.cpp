#include "Application_Windows.h"
#include "Window_Windows.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/Swapchain.h"

#include <windows.h>
#include <windowsx.h>
#include <Shlobj.h>
#undef CreateWindow
#undef max

LRESULT CALLBACK WindowsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	using namespace CE;

	static auto GetKeyMods = []() -> EInputModifier
	{
		EInputModifier Modifiers = EInputModifier::None;

		if (GetKeyState(VK_SHIFT) & 0x8000)
			Modifiers |= EInputModifier::Shift;
		if (GetKeyState(VK_CONTROL) & 0x8000)
			Modifiers |= EInputModifier::Control;
		if (GetKeyState(VK_MENU) & 0x8000)
			Modifiers |= EInputModifier::Alt;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
			Modifiers |= EInputModifier::Super;
		if (GetKeyState(VK_CAPITAL) & 1)
			Modifiers |= EInputModifier::CapsLock;
		if (GetKeyState(VK_NUMLOCK) & 1)
			Modifiers |= EInputModifier::NumLock;

		return Modifiers;
	};

	Window_Windows* Win = static_cast<Window_Windows*>(GetPropW(hwnd, L"CE_WINDOW"));
	if (Win != nullptr)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
		{
			Win->Close();
			return 0;
		}
		case WM_MOUSEACTIVATE:
		{
			// HACK: Postpone cursor disabling when the window was activated by
			//       clicking a caption button
			if (HIWORD(lParam) == WM_LBUTTONDOWN)
			{
				if (LOWORD(lParam) != HTCLIENT)
				{
					Win->InNCAction = true;
					if (Win->CursorMode == ECursorMode::Disabled)
					{
						Win->EnableCursor(false);
					}
				}
			}
			break;
		}
		case WM_CAPTURECHANGED:
		{
			if (lParam == 0 && Win->InNCAction && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			{
				Win->InNCAction = false;
				if (Win->CursorMode == ECursorMode::Disabled)
				{
					Win->DisableCursor(true);
				}
			}
			break;
		}
		case WM_NCLBUTTONDOWN:
		{
			Win->InNCAction = true;
			if (Win->CursorMode == ECursorMode::Disabled)
			{
				Win->EnableCursor(false);
			}
			break;
		}
		case WM_SETFOCUS:
		{
			if (!Win->InNCAction && Win->CursorMode == ECursorMode::Disabled)
			{
				Win->DisableCursor(true);
			}
			break;
		}
		case WM_KILLFOCUS:
		{
			if (!Win->InNCAction && Win->CursorMode == ECursorMode::Disabled)
			{
				Win->EnableCursor(true);
			}
			Engine::Get().InputManager.ReleaseButtons();
			break;
		}
		case WM_SIZE:
		{
			if (!Win->InNCAction && Win->CursorMode == ECursorMode::Disabled)
			{
				Win->UpdateCursorClip(ECursorMode::Disabled);
			}

			UINT Width = LOWORD(lParam);
			UINT Height = HIWORD(lParam);

			SharedPtr<Swapchain> Swapchain = Win->GetSwapchain();
			if (Swapchain != nullptr)
			{
				RenderThread::Enqueue([=](RHIContextImmediate& Context)
					{
						Swapchain->Resize(Width, Height);
					});
			}

			break;
		}
		case WM_MOVE:
		{
			if (!Win->InNCAction && Win->CursorMode == ECursorMode::Disabled)
			{
				Win->UpdateCursorClip(ECursorMode::Disabled);
			}
			break;
		}
		case WM_SETCURSOR:
		{
			if ((Win->CursorMode == ECursorMode::Disabled && Win->CursorDisabled) || Win->CursorMode == ECursorMode::Hidden)
			{
				::SetCursor(LoadCursor(NULL, NULL));
			}
			else
			{
				Win->UpdateCursor();
			}
			break;
		}
		case WM_DROPFILES:
		{
			break;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			bool Pressed = !((HIWORD(lParam) & KF_UP) == KF_UP);
			if (Pressed && (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
			{
				// We ignore Repeat Keys
				break;
			}
			const EInputModifier Mods = GetKeyMods();

			int Scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
			if (!Scancode)
			{
				// NOTE: Some synthetic key messages have a scancode of zero
				// HACK: Map the virtual key back to a usable scancode
				Scancode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
			}

			EInput Input = Application_Windows::ScancodeToInput(Scancode);

			// The Ctrl keys require special handling
			if (wParam == VK_CONTROL)
			{
				if (HIWORD(lParam) & KF_EXTENDED)
				{
					// Right side keys have the extended key bit set
					Input = EInput::Keyboard_Right_Control;
				}
				else
				{
					// NOTE: Alt Gr sends Left Ctrl followed by Right Alt
					// HACK: We only want one event for Alt Gr, so if we detect
					//       this sequence we discard this Left Ctrl message now
					//       and later report Right Alt normally
					MSG next;
					const DWORD time = GetMessageTime();

					if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
					{
						if (next.message == WM_KEYDOWN ||
							next.message == WM_SYSKEYDOWN ||
							next.message == WM_KEYUP ||
							next.message == WM_SYSKEYUP)
						{
							if (next.wParam == VK_MENU &&
								(HIWORD(next.lParam) & KF_EXTENDED) &&
								next.time == time)
							{
								// Next message is Right Alt down so discard this
								break;
							}
						}
					}

					// This is a regular Left Ctrl message
					Input = EInput::Keyboard_Left_Control;
				}
			}
			else if (wParam == VK_PROCESSKEY)
			{
				// IME notifies that keys have been filtered by setting the
				// virtual key-code to VK_PROCESSKEY
				break;
			}

			auto KeyMods = GetKeyMods();
			if (wParam == VK_SHIFT && !Pressed)
			{
				// HACK: Release both Shift keys on Shift up event, as when both
				//       are pressed the first release does not emit any event
				// NOTE: The other half of this is in _glfwPlatformPollEvents

				Engine::Get().InputManager.ProcessInput(Win, EInput::Keyboard_Left_Shift, false, KeyMods);
				Engine::Get().InputManager.ProcessInput(Win, EInput::Keyboard_Right_Shift, false, KeyMods);
			}
			else if (wParam == VK_SNAPSHOT)
			{
				// HACK: Key down is not reported for the Print Screen key
				Engine::Get().InputManager.ProcessInput(Win, Input, true, KeyMods);
				Engine::Get().InputManager.ProcessInput(Win, Input, false, KeyMods);
			}
			else
			{
				Engine::Get().InputManager.ProcessInput(Win, Input, Pressed, KeyMods);
			}

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			EInput Input = EInput::None;
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDBLCLK)
				Input = EInput::Mouse_Left;
			else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK)
				Input = EInput::Mouse_Middle;
			else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MBUTTONDBLCLK)
				Input = EInput::Mouse_Right;
			else if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP || uMsg == WM_XBUTTONDBLCLK)
			{
				int XIndex = GET_XBUTTON_WPARAM(wParam);
				switch (XIndex)
				{
				case 1:
					Input = EInput::Mouse_4;
					break;
				case 2:
					Input = EInput::Mouse_5;
					break;
				case 3:
					Input = EInput::Mouse_6;
					break;
				case 4:
					Input = EInput::Mouse_7;
					break;
				case 5:
					Input = EInput::Mouse_8;
					break;
				default:
					Input = EInput::None;
					break;
				}
			}

			bool Pressed = !(uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_XBUTTONUP);

			EInputModifier KeyMods = GetKeyMods();

			Engine::Get().InputManager.ProcessInput(Win, Input, Pressed, KeyMods);
			break;
		}
		case WM_CHAR:
		case WM_SYSCHAR:
		{
			unsigned short Char = (unsigned short)wParam;
			Engine::Get().InputManager.ProcessInput(Win, Char);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			double ScrollVertical = (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA;

			const EInputModifier Mods = GetKeyMods();
			Engine::Get().InputManager.ProcessInput(Win, EInput::Mouse_Wheel, (float)ScrollVertical, Mods);
			break;
		}
		case WM_MOUSEHWHEEL:
		{
			double ScrollHorizontal = -((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
			//InputListener::InputMouseScroll(Win, 0.0f, ScrollHorizontal);
			//Engine::Get().InputManager.ProcessInput(Win, EInput::Mouse_ScrollX, ScrollHorizontal);
			break;
		}
		case WM_INPUT:
		{
			HRAWINPUT ri = (HRAWINPUT)lParam;

			UINT RawInputDataSize = 0;
			GetRawInputData(ri, RID_INPUT, NULL, &RawInputDataSize, sizeof(RAWINPUTHEADER));
			Array<char> RawInputData;
			RawInputData.AddUninitialized(RawInputDataSize);
			GetRawInputData(ri, RID_INPUT, RawInputData.Data(), &RawInputDataSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* RawInput = (RAWINPUT*)RawInputData.Data();

			int32 MouseX = 0;
			int32 MouseY = 0;
			if (RawInput->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
			{
				//MouseX = RawInput->data.mouse.lLastX - window->win32.lastCursorPosX;
				//MouseY = RawInput->data.mouse.lLastY - window->win32.lastCursorPosY;
			}
			else
			{
				MouseX = RawInput->data.mouse.lLastX;
				MouseY = RawInput->data.mouse.lLastY;
			}

			const EInputModifier Mods = GetKeyMods();

			Engine::Get().InputManager.ProcessInput(Win, EInput::Mouse_X, (float)MouseX, Mods);
			Engine::Get().InputManager.ProcessInput(Win, EInput::Mouse_Y, -(float)MouseY, Mods);

			//window->win32.lastCursorPosX += MouseX;
			//window->win32.lastCursorPosY += MouseY;

			break;
		}
		}
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

namespace CE
{
	constexpr const wchar_t* WINDOW_CLASS_NAME = L"CE_WINDOW_CLASS";

	bool Application_Windows::Initialize()
	{
		m_WindowClassName = Text::Format(L"%s_%p", WINDOW_CLASS_NAME, this);

		WNDCLASSEXW WndClass{};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.lpfnWndProc = &WindowsWindowProc;
		WndClass.lpszClassName = m_WindowClassName.Data();
		WndClass.hIcon = NULL;
		WndClass.hInstance = GetModuleHandleW(NULL);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		return RegisterClassExW(&WndClass) != 0;
	}

	void Application_Windows::Tick(float DeltaTime)
	{
		m_InputValues.clear();

		MSG Msg{};
		BOOL PeekResult = PeekMessageW(&Msg, NULL, 0, 0, PM_REMOVE);
		if (PeekResult > 0)
		{
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		else if (PeekResult < 0)
		{
			// Error
		}
		else
		{
			// Program Exit
		}
	}

	void Application_Windows::Terminate()
	{
		UnregisterClassW(m_WindowClassName.Data(), GetModuleHandleW(NULL));
	}

	static BOOL CALLBACK IterateMonitors(HMONITOR monitor, HDC, LPRECT, LPARAM DWData)
	{
		Array<Monitor>& Monitors = *reinterpret_cast<Array<Monitor>*>(DWData);

		MONITORINFO MInfo = {};
		MInfo.cbSize = sizeof(MONITORINFO);
		if (!::GetMonitorInfo(monitor, &MInfo))
			return TRUE;
		Vec2f MainPos = Vec2f((float)MInfo.rcMonitor.left, (float)MInfo.rcMonitor.top);
		Vec2f MainSize = Vec2f((float)(MInfo.rcMonitor.right - MInfo.rcMonitor.left), (float)(MInfo.rcMonitor.bottom - MInfo.rcMonitor.top));
		Vec2f WorkPos = Vec2f((float)MInfo.rcWork.left, (float)MInfo.rcWork.top);
		Vec2f WorkSize = Vec2f((float)(MInfo.rcWork.right - MInfo.rcWork.left), (float)(MInfo.rcWork.bottom - MInfo.rcWork.top));
		//Info.DPIScale = ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);	//@TODO: Handle Dpi Scale
		Monitor Info = Monitor(monitor, MainPos, MainSize, WorkPos, WorkSize);

		if (MInfo.dwFlags & MONITORINFOF_PRIMARY)
		{
			Monitors.Insert(0, Info);
		}
		else
		{
			Monitors.Add(Info);
		}
		return TRUE;
	}

	Array<Monitor> Application_Windows::GetMonitors() const
	{
		Array<Monitor> Result;
		::EnumDisplayMonitors(NULL, NULL, IterateMonitors, (LPARAM)&Result);
		return Result;
	}

	SharedPtr<Window> Application_Windows::CreateWindow(const Text& Title, const Vec2ui& Size, EWindowDisplayMode DisplayMode /*= EWindowDisplayMode::Default*/)
	{
		return CreateWindow(Title, Size, Vec2ui(CW_USEDEFAULT, CW_USEDEFAULT), DisplayMode);
	}

	SharedPtr<Window> Application_Windows::CreateWindow(const Text& Title, const Vec2ui& Size, const Vec2ui& Pos, EWindowDisplayMode DisplayMode /*= EWindowDisplayMode::Default*/)
	{
		HWND NativeWindow = ::CreateWindowExW(0, m_WindowClassName.Data(), Title.Data(), WS_OVERLAPPEDWINDOW, Pos.X, Pos.Y, Size.X, Size.Y, NULL, NULL, GetModuleHandle(NULL), NULL);
		SharedPtr<Window_Windows> NewWindow = SharedPtr<Window_Windows>(new Window_Windows(NativeWindow));
		::UpdateWindow(NativeWindow);
		::SetProp(NativeWindow, L"CE_WINDOW", NewWindow.Get());
		NewWindow->SetDisplayMode(DisplayMode);
		return NewWindow;
	}

	Window* Application_Windows::GetForegroundWindow() const
	{
		HWND ForegroundWindow = ::GetForegroundWindow();
		if (ForegroundWindow != NULL)
		{
			return static_cast<Window_Windows*>(GetPropW(ForegroundWindow, L"CE_WINDOW"));
		}
		return nullptr;
	}

	Window* Application_Windows::GetActiveWindow() const
	{
		HWND ActiveWindow = ::GetActiveWindow();
		if (ActiveWindow != NULL)
		{
			return static_cast<Window_Windows*>(GetPropW(ActiveWindow, L"CE_WINDOW"));
		}
		return nullptr;
	}

	Window* Application_Windows::GetWindowFromPoint(const Vec2f& ScreenPosition) const
	{
		HWND WindowAtPoint = ::WindowFromPoint(POINT{ (LONG)ScreenPosition.X, (LONG)ScreenPosition.Y });
		if (WindowAtPoint != NULL)
		{
			return static_cast<Window_Windows*>(GetPropW(WindowAtPoint, L"CE_WINDOW"));
		}
		return nullptr;
	}

	Monitor Application_Windows::GetMonitorFromWindow(Window* Window) const
	{
		HMONITOR WindowMonitor = ::MonitorFromWindow((HWND)Window->GetNativeHandle(), MONITOR_DEFAULTTONEAREST);
		Array<Monitor> Monitors = GetMonitors();
		for (auto& Monitor : Monitors)
		{
			if (Monitor.GetNativeHandle() == WindowMonitor)
			{
				return Monitor;
			}
		}
		return Monitor(nullptr, Vec2f(0.0f), Vec2f(0.0f), Vec2f(0.0f), Vec2f(0.0f));
	}

	bool Application_Windows::IsInputPressed(EInput Input) const
	{
		int VK = InputToVirtualKey(Input);
		return ::GetAsyncKeyState(VK) & 0x8000;
	}

	String Application_Windows::NameOfInput(EInput Input) const
	{
		return "UNKNOWN";
	}

	Vec2<long> Application_Windows::GetCursorPosition(Window* Origin /*= nullptr*/) const
	{
		POINT NPoint{ 0,0 };
		::GetCursorPos(&NPoint);
		if (Origin != nullptr)
		{
			Window_Windows* COrigin = dynamic_cast<Window_Windows*>(Origin);
			if (COrigin == nullptr)
			{
				CE_LOG(ELog::Warning, "Origin is not a Windows Window");
				return Vec2<long>(0);
			}
			::ScreenToClient((HWND)COrigin->GetNativeHandle(), &NPoint);
		}
		return Vec2<long>(NPoint.x, NPoint.y);
	}

	void Application_Windows::SetCursorPosition(const Vec2<long>& Pos, Window* Origin /*= nullptr*/) const
	{
		POINT NPoint{ Pos.X, Pos.Y };
		if (Origin != nullptr)
		{
			Window_Windows* COrigin = dynamic_cast<Window_Windows*>(Origin);
			if (COrigin == nullptr)
			{
				CE_LOG(ELog::Warning, "Origin is not a Windows Window");
				return;
			}
			::ClientToScreen((HWND)COrigin->GetNativeHandle(), &NPoint);
		}
		::SetCursorPos(NPoint.x, NPoint.y);
	}

	int Application_Windows::InputToScancode(EInput Input)
	{
		switch (Input)
		{
		case EInput::Keyboard_0:			return 0x00B;
		case EInput::Keyboard_1:			return 0x002;
		case EInput::Keyboard_2:			return 0x003;
		case EInput::Keyboard_3:			return 0x004;
		case EInput::Keyboard_4:			return 0x005;
		case EInput::Keyboard_5:			return 0x006;
		case EInput::Keyboard_6:			return 0x007;
		case EInput::Keyboard_7:			return 0x008;
		case EInput::Keyboard_8:			return 0x009;
		case EInput::Keyboard_9:			return 0x00A;
		case EInput::Keyboard_A:			return 0x01E;
		case EInput::Keyboard_B:			return 0x030;
		case EInput::Keyboard_C:			return 0x02E;
		case EInput::Keyboard_D:			return 0x020;
		case EInput::Keyboard_E:			return 0x012;
		case EInput::Keyboard_F:			return 0x021;
		case EInput::Keyboard_G:			return 0x022;
		case EInput::Keyboard_H:			return 0x023;
		case EInput::Keyboard_I:			return 0x017;
		case EInput::Keyboard_J:			return 0x024;
		case EInput::Keyboard_K:			return 0x025;
		case EInput::Keyboard_L:			return 0x026;
		case EInput::Keyboard_M:			return 0x032;
		case EInput::Keyboard_N:			return 0x031;
		case EInput::Keyboard_O:			return 0x018;
		case EInput::Keyboard_P:			return 0x019;
		case EInput::Keyboard_Q:			return 0x010;
		case EInput::Keyboard_R:			return 0x013;
		case EInput::Keyboard_S:			return 0x01F;
		case EInput::Keyboard_T:			return 0x014;
		case EInput::Keyboard_U:			return 0x016;
		case EInput::Keyboard_V:			return 0x02F;
		case EInput::Keyboard_W:			return 0x011;
		case EInput::Keyboard_X:			return 0x02D;
		case EInput::Keyboard_Y:			return 0x015;
		case EInput::Keyboard_Z:			return 0x02C;
		case EInput::Keyboard_Apostrophe:		return 0x028;
		case EInput::Keyboard_Backslash:		return 0x02B;
		case EInput::Keyboard_Comma:			return 0x033;
		case EInput::Keyboard_Equal:			return 0x00D;
		case EInput::Keyboard_Grave_Accent:	return 0x029;
		case EInput::Keyboard_Left_Bracket:	return 0x01A;
		case EInput::Keyboard_Minus:			return 0x00C;
		case EInput::Keyboard_Period:			return 0x034;
		case EInput::Keyboard_Right_Bracket:	return 0x01B;
		case EInput::Keyboard_Semicolon:		return 0x027;
		case EInput::Keyboard_Slash:			return 0x035;
		case EInput::Keyboard_World_2:			return 0x056;
		case EInput::Keyboard_Backspace:		return 0x00E;
		case EInput::Keyboard_Delete:			return 0x153;
		case EInput::Keyboard_End:				return 0x14F;
		case EInput::Keyboard_Enter:			return 0x01C;
		case EInput::Keyboard_Escape:			return 0x001;
		case EInput::Keyboard_Home:			return 0x147;
		case EInput::Keyboard_Insert:			return 0x152;
		case EInput::Keyboard_Menu:			return 0x15D;
		case EInput::Keyboard_Page_Down:		return 0x151;
		case EInput::Keyboard_Page_Up:			return 0x149;
		case EInput::Keyboard_Pause:			return 0x045;
		case EInput::Keyboard_Space:			return 0x039;
		case EInput::Keyboard_Tab:				return 0x00F;
		case EInput::Keyboard_Caps_Lock:		return 0x03A;
		case EInput::Keyboard_Num_Lock:		return 0x145;
		case EInput::Keyboard_Scroll_Lock:		return 0x046;
		case EInput::Keyboard_F1:				return 0x03B;
		case EInput::Keyboard_F2:				return 0x03C;
		case EInput::Keyboard_F3:				return 0x03D;
		case EInput::Keyboard_F4:				return 0x03E;
		case EInput::Keyboard_F5:				return 0x03F;
		case EInput::Keyboard_F6:				return 0x040;
		case EInput::Keyboard_F7:				return 0x041;
		case EInput::Keyboard_F8:				return 0x042;
		case EInput::Keyboard_F9:				return 0x043;
		case EInput::Keyboard_F10:				return 0x044;
		case EInput::Keyboard_F11:				return 0x057;
		case EInput::Keyboard_F12:				return 0x058;
		case EInput::Keyboard_F13:				return 0x064;
		case EInput::Keyboard_F14:				return 0x065;
		case EInput::Keyboard_F15:				return 0x066;
		case EInput::Keyboard_F16:				return 0x067;
		case EInput::Keyboard_F17:				return 0x068;
		case EInput::Keyboard_F18:				return 0x069;
		case EInput::Keyboard_F19:				return 0x06A;
		case EInput::Keyboard_F20:				return 0x06B;
		case EInput::Keyboard_F21:				return 0x06C;
		case EInput::Keyboard_F22:				return 0x06D;
		case EInput::Keyboard_F23:				return 0x06E;
		case EInput::Keyboard_F24:				return 0x076;
		case EInput::Keyboard_Left_Alt:		return 0x038;
		case EInput::Keyboard_Left_Control:	return 0x01D;
		case EInput::Keyboard_Left_Shift:		return 0x02A;
		case EInput::Keyboard_Left_Super:		return 0x15B;
		case EInput::Keyboard_Print_Screen:	return 0x137;
		case EInput::Keyboard_Right_Alt:		return 0x138;
		case EInput::Keyboard_Right_Control:	return 0x11D;
		case EInput::Keyboard_Right_Shift:		return 0x036;
		case EInput::Keyboard_Right_Super:		return 0x15C;
		case EInput::Keyboard_Down:			return 0x150;
		case EInput::Keyboard_Left:			return 0x14B;
		case EInput::Keyboard_Right:			return 0x14D;
		case EInput::Keyboard_Up:				return 0x148;
		case EInput::Keyboard_Keypad_0:		return 0x052;
		case EInput::Keyboard_Keypad_1:		return 0x04F;
		case EInput::Keyboard_Keypad_2:		return 0x050;
		case EInput::Keyboard_Keypad_3:		return 0x051;
		case EInput::Keyboard_Keypad_4:		return 0x04B;
		case EInput::Keyboard_Keypad_5:		return 0x04C;
		case EInput::Keyboard_Keypad_6:		return 0x04D;
		case EInput::Keyboard_Keypad_7:		return 0x047;
		case EInput::Keyboard_Keypad_8:		return 0x048;
		case EInput::Keyboard_Keypad_9:		return 0x049;
		case EInput::Keyboard_Keypad_Add:		return 0x04E;
		case EInput::Keyboard_Keypad_Decimal:	return 0x053;
		case EInput::Keyboard_Keypad_Divide:	return 0x135;
		case EInput::Keyboard_Keypad_Enter:	return 0x11C;
		case EInput::Keyboard_Keypad_Equal:	return 0x059;
		case EInput::Keyboard_Keypad_Multiply:	return 0x037;
		case EInput::Keyboard_Keypad_Subtract:	return 0x04A;
		}

		return 0;
	}

	EInput Application_Windows::ScancodeToInput(int Scancode)
	{
		switch (Scancode)
		{
		case 0x00B: return EInput::Keyboard_0;
		case 0x002: return EInput::Keyboard_1;
		case 0x003: return EInput::Keyboard_2;
		case 0x004: return EInput::Keyboard_3;
		case 0x005: return EInput::Keyboard_4;
		case 0x006: return EInput::Keyboard_5;
		case 0x007: return EInput::Keyboard_6;
		case 0x008: return EInput::Keyboard_7;
		case 0x009: return EInput::Keyboard_8;
		case 0x00A: return EInput::Keyboard_9;
		case 0x01E: return EInput::Keyboard_A;
		case 0x030: return EInput::Keyboard_B;
		case 0x02E: return EInput::Keyboard_C;
		case 0x020: return EInput::Keyboard_D;
		case 0x012: return EInput::Keyboard_E;
		case 0x021: return EInput::Keyboard_F;
		case 0x022: return EInput::Keyboard_G;
		case 0x023: return EInput::Keyboard_H;
		case 0x017: return EInput::Keyboard_I;
		case 0x024: return EInput::Keyboard_J;
		case 0x025: return EInput::Keyboard_K;
		case 0x026: return EInput::Keyboard_L;
		case 0x032: return EInput::Keyboard_M;
		case 0x031: return EInput::Keyboard_N;
		case 0x018: return EInput::Keyboard_O;
		case 0x019: return EInput::Keyboard_P;
		case 0x010: return EInput::Keyboard_Q;
		case 0x013: return EInput::Keyboard_R;
		case 0x01F: return EInput::Keyboard_S;
		case 0x014: return EInput::Keyboard_T;
		case 0x016: return EInput::Keyboard_U;
		case 0x02F: return EInput::Keyboard_V;
		case 0x011: return EInput::Keyboard_W;
		case 0x02D: return EInput::Keyboard_X;
		case 0x015: return EInput::Keyboard_Y;
		case 0x02C: return EInput::Keyboard_Z;
		case 0x028: return EInput::Keyboard_Apostrophe;
		case 0x02B: return EInput::Keyboard_Backslash;
		case 0x033: return EInput::Keyboard_Comma;
		case 0x00D: return EInput::Keyboard_Equal;
		case 0x029: return EInput::Keyboard_Grave_Accent;
		case 0x01A: return EInput::Keyboard_Left_Bracket;
		case 0x00C: return EInput::Keyboard_Minus;
		case 0x034: return EInput::Keyboard_Period;
		case 0x01B: return EInput::Keyboard_Right_Bracket;
		case 0x027: return EInput::Keyboard_Semicolon;
		case 0x035: return EInput::Keyboard_Slash;
		case 0x056: return EInput::Keyboard_World_2;
		case 0x00E: return EInput::Keyboard_Backspace;
		case 0x153: return EInput::Keyboard_Delete;
		case 0x14F: return EInput::Keyboard_End;
		case 0x01C: return EInput::Keyboard_Enter;
		case 0x001: return EInput::Keyboard_Escape;
		case 0x147: return EInput::Keyboard_Home;
		case 0x152: return EInput::Keyboard_Insert;
		case 0x15D: return EInput::Keyboard_Menu;
		case 0x151: return EInput::Keyboard_Page_Down;
		case 0x149: return EInput::Keyboard_Page_Up;
		case 0x045: return EInput::Keyboard_Pause;
		case 0x039: return EInput::Keyboard_Space;
		case 0x00F: return EInput::Keyboard_Tab;
		case 0x03A: return EInput::Keyboard_Caps_Lock;
		case 0x145: return EInput::Keyboard_Num_Lock;
		case 0x046: return EInput::Keyboard_Scroll_Lock;
		case 0x03B: return EInput::Keyboard_F1;
		case 0x03C: return EInput::Keyboard_F2;
		case 0x03D: return EInput::Keyboard_F3;
		case 0x03E: return EInput::Keyboard_F4;
		case 0x03F: return EInput::Keyboard_F5;
		case 0x040: return EInput::Keyboard_F6;
		case 0x041: return EInput::Keyboard_F7;
		case 0x042: return EInput::Keyboard_F8;
		case 0x043: return EInput::Keyboard_F9;
		case 0x044: return EInput::Keyboard_F10;
		case 0x057: return EInput::Keyboard_F11;
		case 0x058: return EInput::Keyboard_F12;
		case 0x064: return EInput::Keyboard_F13;
		case 0x065: return EInput::Keyboard_F14;
		case 0x066: return EInput::Keyboard_F15;
		case 0x067: return EInput::Keyboard_F16;
		case 0x068: return EInput::Keyboard_F17;
		case 0x069: return EInput::Keyboard_F18;
		case 0x06A: return EInput::Keyboard_F19;
		case 0x06B: return EInput::Keyboard_F20;
		case 0x06C: return EInput::Keyboard_F21;
		case 0x06D: return EInput::Keyboard_F22;
		case 0x06E: return EInput::Keyboard_F23;
		case 0x076: return EInput::Keyboard_F24;
		case 0x038: return EInput::Keyboard_Left_Alt;
		case 0x01D: return EInput::Keyboard_Left_Control;
		case 0x02A: return EInput::Keyboard_Left_Shift;
		case 0x15B: return EInput::Keyboard_Left_Super;
		case 0x137: return EInput::Keyboard_Print_Screen;
		case 0x138: return EInput::Keyboard_Right_Alt;
		case 0x11D: return EInput::Keyboard_Right_Control;
		case 0x036: return EInput::Keyboard_Right_Shift;
		case 0x15C: return EInput::Keyboard_Right_Super;
		case 0x150: return EInput::Keyboard_Down;
		case 0x14B: return EInput::Keyboard_Left;
		case 0x14D: return EInput::Keyboard_Right;
		case 0x148: return EInput::Keyboard_Up;
		case 0x052: return EInput::Keyboard_Keypad_0;
		case 0x04F: return EInput::Keyboard_Keypad_1;
		case 0x050: return EInput::Keyboard_Keypad_2;
		case 0x051: return EInput::Keyboard_Keypad_3;
		case 0x04B: return EInput::Keyboard_Keypad_4;
		case 0x04C: return EInput::Keyboard_Keypad_5;
		case 0x04D: return EInput::Keyboard_Keypad_6;
		case 0x047: return EInput::Keyboard_Keypad_7;
		case 0x048: return EInput::Keyboard_Keypad_8;
		case 0x049: return EInput::Keyboard_Keypad_9;
		case 0x04E: return EInput::Keyboard_Keypad_Add;
		case 0x053: return EInput::Keyboard_Keypad_Decimal;
		case 0x135: return EInput::Keyboard_Keypad_Divide;
		case 0x11C: return EInput::Keyboard_Keypad_Enter;
		case 0x059: return EInput::Keyboard_Keypad_Equal;
		case 0x037: return EInput::Keyboard_Keypad_Multiply;
		case 0x04A: return EInput::Keyboard_Keypad_Subtract;
		}

		return EInput::None;
	}

	int Application_Windows::InputToVirtualKey(EInput Input)
	{
		int Scancode = InputToScancode(Input);
		if (Scancode != 0)
		{
			return MapVirtualKeyW(Scancode, MAPVK_VSC_TO_VK);
		}

		switch (Input)
		{
		case EInput::Mouse_Left:
			return VK_LBUTTON;
		case EInput::Mouse_Middle:
			return VK_MBUTTON;
		case EInput::Mouse_Right:
			return VK_RBUTTON;
		case EInput::Mouse_4:
			return VK_XBUTTON1;
		case EInput::Mouse_5:
			return VK_XBUTTON2;
		}

		return 0;
	}
}
