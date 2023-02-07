#include "CardinalEngine-Editor/ImGuiInterface/ImGuiEngine.h"
#include "CardinalEngine-Editor/ImGuiInterface/ImGui/imgui_internal.h"
#include "CardinalEngine-Editor/EngineEditor/EngineEditor.h"

struct InputTextCallback_UserData
{
	CE::String* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		CE::String* str = user_data->Str;
		IM_ASSERT(data->Buf == str->Data());
		if (data->BufTextLen > str->Size())
		{
			str->Append('\0', data->BufTextLen - str->Size());
		}
		else if (data->BufTextLen < str->Size())
		{
			str->RemoveAt(data->BufTextLen, str->Size() - data->BufTextLen);
		}
		data->Buf = (char*)str->Data();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGui::InputText(const char* label, CE::String* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(label, (char*)str->Data(), str->Reserved() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const char* label, CE::String* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextMultiline(label, (char*)str->Data(), str->Reserved() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, CE::String* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextWithHint(label, hint, (char*)str->Data(), str->Reserved() + 1, flags, InputTextCallback, &cb_user_data);
}

ImVec2 ImGui::CalcButtonSize(const char* label, const ImVec2& size_arg /*= ImVec2(0, 0)*/, ImGuiButtonFlags flags /*= 0*/)
{
	ImGuiWindow* window = GetCurrentWindow();

	ImGuiContext& g = *ImGui::GetCurrentContext();
	const ImGuiStyle& style = g.Style;
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	return CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
}

void ImGui::Image(const CE::SharedPtr<CE::RHIImage>& user_texture_id, const ImVec2& size, const ImVec2& uv0 /*= ImVec2(0, 0)*/, const ImVec2& uv1 /*= ImVec2(1, 1)*/, const ImVec4& tint_col /*= ImVec4(1, 1, 1, 1)*/, const ImVec4& border_col /*= ImVec4(0, 0, 0, 0)*/)
{
	CE::EngineEditor::Get().ImGuiInterface.OneTimeImages.AddUnique(user_texture_id);
	ImGui::Image((ImTextureID)user_texture_id.Get(), size, uv0, uv1, tint_col, border_col);
}

bool ImGui::ImageButton(const char* str_id, const CE::SharedPtr<CE::RHIImage>& user_texture_id, const ImVec2& size, const ImVec2& uv0 /*= ImVec2(0, 0)*/, const ImVec2& uv1 /*= ImVec2(1, 1)*/, const ImVec4& bg_col /*= ImVec4(0, 0, 0, 0)*/, const ImVec4& tint_col /*= ImVec4(1, 1, 1, 1)*/)
{
	CE::EngineEditor::Get().ImGuiInterface.OneTimeImages.AddUnique(user_texture_id);
	return ImGui::ImageButton(str_id, (ImTextureID)user_texture_id.Get(), size, uv0, uv1, bg_col, tint_col);
}
