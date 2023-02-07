#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Application/Window.h"

#include <windows.h>

namespace CE
{
	class Window_Windows : public Window
	{
	public:
		Window_Windows(void* NativeHandle);

		virtual ~Window_Windows();

	public:
		virtual void Close() override;

		virtual bool IsCloseRequested() const override
		{
			return CloseRequested;
		}

		virtual void SetTitle(const Text& Title) override;

		virtual Text GetTitle() const override;

		virtual Vec2ui GetSize() const override;

		virtual void SetDisplayMode(EWindowDisplayMode Mode) override;

		virtual EWindowDisplayMode GetDisplayMode() const override;

		virtual void Focus() override;

		virtual void RequestFocus() override;

		virtual bool HasFocus() const override;

		virtual void SetOpacity(float Opacity) override;

		virtual float GetOpacity() const override;

		virtual void SetStyleFlag(EWindowStyleFlag Flag, bool Value) override;

		virtual bool GetStyleFlag(EWindowStyleFlag Flag) const override;

		virtual void SetCursorMode(ECursorMode Mode) override;

		virtual ECursorMode GetCursorMode() const override;

		virtual void SetCursor(ECursor Cursor) override;

		virtual ECursor GetCursor() const override;

		virtual void* GetNativeHandle() const override;

	public:
		void DisableCursor(bool SaveCursorPos);
		void EnableCursor(bool RestoreCursorPos);
		void UpdateCursorClip(ECursorMode Mode);
		void UpdateCursor();

	private:
		void UpdateWindowStyle(unsigned long Style, unsigned long ExStyle);

	public:
		void* NativeHandle = nullptr;
		bool CloseRequested = false;

		ECursor Cursor = ECursor::Arrow;
		ECursorMode CursorMode = ECursorMode::Default;
		Vec2ui RestoreCursorPos;

		bool InNCAction = false;
		bool CursorDisabled = false;
	};
}
