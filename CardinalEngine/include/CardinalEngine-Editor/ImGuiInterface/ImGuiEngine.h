#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "ImGui/imgui.h"

namespace CE
{
	class RHIImage;
}

namespace ImGui
{
	// ImGui::InputText() with CE::String
	// Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
	IMGUI_API bool InputText(const char* label, CE::String* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool InputTextMultiline(const char* label, CE::String* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool InputTextWithHint(const char* label, const char* hint, CE::String* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	IMGUI_API ImVec2 CalcButtonSize(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);

	IMGUI_API void Image(const CE::SharedPtr<CE::RHIImage>& user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	IMGUI_API bool ImageButton(const char* str_id, const CE::SharedPtr<CE::RHIImage>& user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
}
