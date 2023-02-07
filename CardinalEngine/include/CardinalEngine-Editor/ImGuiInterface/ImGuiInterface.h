#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../../CardinalEngine/Application/Keys.h"

struct ImGuiViewport;
namespace CE
{
	class Window;
	class RHIBuffer;
	class RHIImage;
	class RHIPipeline_Graphic;
	class Sampler;

	class CE_API ImGuiEditorStyle
	{
	public:
		ImGuiEditorStyle() = delete;
	
		static float ImGuiEditorStyle::MainHue;
		static Color ImGuiEditorStyle::TextColor;
		static Color ImGuiEditorStyle::TextColor_Disabled;
		static Color ImGuiEditorStyle::BackgroundColor;
		static Color ImGuiEditorStyle::ScrollBarColor;
		static Color ImGuiEditorStyle::ScrollBarColor_Active;
		static Color ImGuiEditorStyle::BorderColor;
		static Color ImGuiEditorStyle::AccentColor;
		static Color ImGuiEditorStyle::AccentColor_Hovered;
		static Color ImGuiEditorStyle::AccentColor_Active;
	};

	class CE_API ImGuiInterface : public Singleton
	{
	public:
		void Initialize();

		void SetupStyle();

		void BeginFrame(float DeltaTime);

		void EndFrame();

		void Terminate();

		void SetWindow(const SharedPtr<Window>& NewWindow);

		SharedPtr<Window> GetWindow() const;

	public:
		void OnKeyboardKey(Window* Window, EInput Key, bool Pressed, EInputModifier Modifier);
		
		void OnMouseButton(Window* Window, EInput Button, bool Pressed, EInputModifier Modifier);
		
		void OnMouseScroll(Window* Window, double ScrollVertical, double ScrollHorizontal);
		
		void OnChar(Window* Window, unsigned short Char);

	private:
		const String& GetImGuiShaderSource();

		void RenderImGuiViewport(ImGuiViewport* Viewport);

	public:
		Array<SharedPtr<RHIImage>> OneTimeImages;

	private:
		WeakPtr<Window> m_Window;

		struct ImGuiDrawBuffers
		{
			SharedPtr<RHIBuffer> VertexBuffer;
			SharedPtr<RHIBuffer> IndexBuffer;
			SharedPtr<RHIBuffer> UniformBuffer;
		};
		SharedPtr<Sampler> m_Sampler;
		SharedPtr<RHIImage> m_DefaultFontImage;
		SharedPtr<RHIPipeline_Graphic> m_Pipeline;
	};
}
