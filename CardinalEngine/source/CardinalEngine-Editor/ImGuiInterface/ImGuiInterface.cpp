#include "CardinalEngine-Editor/ImGuiInterface/ImGuiInterface.h"

#include "CardinalEngine-Editor/ImGuiInterface/ImGui/imgui.h"
#include "CardinalEngine/FileSystem/Paths.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Application/Application.h"
#include "CardinalEngine/Application/Window.h"

#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/ShaderCompiler.h"
#include "CardinalEngine/Rendering/RHI/Pipeline.h"
#include "CardinalEngine/Rendering/RHI/Sampler.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/Swapchain.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"

namespace CE
{
	float ImGuiEditorStyle::MainHue = 0.784f;	// Default Purple
	//float ImGuiEditorStyle::MainHue = 0.05f;		// Default Orange
	//float ImGuiEditorStyle::MainHue = 0.568f;	// Default Blue
	Color ImGuiEditorStyle::TextColor = Color::White;
	Color ImGuiEditorStyle::TextColor_Disabled = 0x979797FF;
	Color ImGuiEditorStyle::BackgroundColor = 0x131313FF;
	Color ImGuiEditorStyle::ScrollBarColor = 0x525255FF;
	Color ImGuiEditorStyle::ScrollBarColor_Active = 0x5A5A5FFF;
	Color ImGuiEditorStyle::BorderColor = 0x4E4E4EFF;
	Color ImGuiEditorStyle::AccentColor = Color::FromHSV(MainHue, 0.074f, 0.215f);
	Color ImGuiEditorStyle::AccentColor_Hovered = Color::FromHSV(ImGuiEditorStyle::MainHue, 0.878f, 0.925f);
	Color ImGuiEditorStyle::AccentColor_Active = Color::FromHSV(ImGuiEditorStyle::MainHue, 1.0f, 0.784f);

	void ImGuiInterface::Initialize()
	{
		// Create ImGui Context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();


		// Setup Config
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsResizeFromEdges = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;


		// Create the ImGui Layout Ini File
		std::filesystem::path EditorLayoutPath = Paths::EditorWorkDirectory() / "Configs/";
		std::filesystem::create_directories(EditorLayoutPath);	//@NOTE: If the directories don't exist ImGui will not save anything
		static std::string EditorLayoutIni = (EditorLayoutPath / "EditorLayout.ini").u8string();
		io.IniFilename = EditorLayoutIni.c_str();

		io.Fonts->AddFontFromFileTTF((Paths::EditorResources() / "Source/Fonts/arial.ttf").u8string().c_str(), 15);

		//@TODO: Load ImGui Data from File
		///ImGuiContext* ImGuiCon = ImGui::GetCurrentContext();
		///ImGuiCon->SettingsTables;
		///ImGuiCon->SettingsWindows;
		///ImGuiCon->DockContext.NodesSettings;


		// Setup Backend
		io.BackendPlatformName = "CardinalEngine";
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

		// Setup ImGui Keymap
		io.KeyMap[ImGuiKey_Tab] = (int)EInput::Keyboard_Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = (int)EInput::Keyboard_Left;
		io.KeyMap[ImGuiKey_RightArrow] = (int)EInput::Keyboard_Right;
		io.KeyMap[ImGuiKey_UpArrow] = (int)EInput::Keyboard_Up;
		io.KeyMap[ImGuiKey_DownArrow] = (int)EInput::Keyboard_Down;
		io.KeyMap[ImGuiKey_PageUp] = (int)EInput::Keyboard_Page_Up;
		io.KeyMap[ImGuiKey_PageDown] = (int)EInput::Keyboard_Page_Down;
		io.KeyMap[ImGuiKey_Home] = (int)EInput::Keyboard_Home;
		io.KeyMap[ImGuiKey_End] = (int)EInput::Keyboard_End;
		io.KeyMap[ImGuiKey_Insert] = (int)EInput::Keyboard_Insert;
		io.KeyMap[ImGuiKey_Delete] = (int)EInput::Keyboard_Delete;
		io.KeyMap[ImGuiKey_Backspace] = (int)EInput::Keyboard_Backspace;
		io.KeyMap[ImGuiKey_Space] = (int)EInput::Keyboard_Space;
		io.KeyMap[ImGuiKey_Enter] = (int)EInput::Keyboard_Enter;
		io.KeyMap[ImGuiKey_Escape] = (int)EInput::Keyboard_Escape;
		io.KeyMap[ImGuiKey_KeyPadEnter] = (int)EInput::Keyboard_Keypad_Enter;
		io.KeyMap[ImGuiKey_A] = (int)EInput::Keyboard_A;
		io.KeyMap[ImGuiKey_C] = (int)EInput::Keyboard_C;
		io.KeyMap[ImGuiKey_V] = (int)EInput::Keyboard_V;
		io.KeyMap[ImGuiKey_X] = (int)EInput::Keyboard_X;
		io.KeyMap[ImGuiKey_Y] = (int)EInput::Keyboard_Y;
		io.KeyMap[ImGuiKey_Z] = (int)EInput::Keyboard_Z;


		// Setup ImGui Viewports if enabled
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			CE_ASSERT(false, "ImGui Viewports currently not supported");

			ImGui::GetStyle().WindowRounding = 0.0f;
			ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;

			io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
			// Set ImGui Viewport Platform Callbacks

			io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
			// Set ImGui Viewport Renderer Callbacks
		}


		// Reference to Dynamic RHI
		DynamicRHI& DRHI = Engine::Get().DynamicRHI();


		// Compile Shaders
		const String& ShaderSource = GetImGuiShaderSource();
		Array<char> VertexShaderCode;
		Array<char> FragmentShaderCode;

		ShaderCompiler SCompiler;
		if (DRHI.Type() == ERHIType::Vulkan)
		{
			SCompiler.CompileToSPIRV(EShaderStage::Vertex, EShaderSourceLanguage::HLSL, ShaderSource.Data(), ShaderSource.Size(), "VShader", nullptr, VertexShaderCode);
			SCompiler.CompileToSPIRV(EShaderStage::Fragment, EShaderSourceLanguage::HLSL, ShaderSource.Data(), ShaderSource.Size(), "FShader", nullptr, FragmentShaderCode);
		}
		else
		{
			SCompiler.CompileToHLSL(EShaderStage::Vertex, EShaderSourceLanguage::HLSL, ShaderSource.Data(), ShaderSource.Size(), "VShader", nullptr, VertexShaderCode);
			SCompiler.CompileToHLSL(EShaderStage::Fragment, EShaderSourceLanguage::HLSL, ShaderSource.Data(), ShaderSource.Size(), "FShader", nullptr, FragmentShaderCode);
		}


		// Create Shaders
		RHIPipeline_GraphicShaders PipelineShaders;
		PipelineShaders.VertexShader = DRHI.CreateVertexShader(VertexShaderCode.Data(), VertexShaderCode.Size(), "VShader");
		PipelineShaders.FragmentShader = DRHI.CreateFragmentShader(FragmentShaderCode.Data(), FragmentShaderCode.Size(), "FShader");

		// Create Pipeline
		RHIPipeline_GraphicDescription PipelineDescription;
		PipelineDescription.RenderTargetFormats.Emplace(DRHI.Config().SwapchainImageFormat);
		PipelineDescription.VertexInputLayout = {
			ShaderVertexInput(0, EDataFormat::RG32_Float),
			ShaderVertexInput(1, EDataFormat::RG32_Float),
			ShaderVertexInput(2, EDataFormat::RGBA8_UNorm),
		};
		PipelineDescription.ResourceBindings = {
			ShaderResourceBinding(0, EShaderResourceType::UniformBuffer),
			ShaderResourceBinding(1, EShaderResourceType::SampledImage),
			ShaderResourceBinding(2, EShaderResourceType::Sampler),
		};
		PipelineDescription.Rasterizer.ScissorEnabled = true;
		PipelineDescription.Rasterizer.DepthClampEnabled = true;
		PipelineDescription.Rasterizer.CullMode = ECullMode::None;

		RenderTargetBlend& Blend = PipelineDescription.BlendState.RenderTargetBlends.EmplaceRef();
		Blend.BlendEnabled = true;
		Blend.SrcBlend = EBlendFactor::SrcAlpha;
		Blend.DstBlend = EBlendFactor::InvSrcAlpha;
		Blend.BlendOp = EBlendOperation::Add;
		Blend.SrcBlendAlpha = EBlendFactor::One;
		Blend.DstBlendAlpha = EBlendFactor::InvSrcAlpha;
		Blend.BlendOpAlpha = EBlendOperation::Add;
		Blend.LogicOp = ELogicOperation::Copy;
		Blend.WriteMask = EBlendMask::AllComponents;

		m_Pipeline = DRHI.CreatePipelineGraphic(PipelineShaders, PipelineDescription);


		// Create Sampler
		SamplerDescription SamplerDesc;
		m_Sampler = DRHI.CreateSampler(SamplerDesc);

		// Get Font Texture pixels
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		// Create Default Font Image
		RHIImageDescription FontImageDesc;
		FontImageDesc.Width = width;
		FontImageDesc.Height = height;
		FontImageDesc.Usage = ERHIImageUsage::ShaderResource;
		FontImageDesc.Access = ERHIAccess::GPURead_CPUWrite;	//@TODO: Use Staging for the Font Image
		m_DefaultFontImage = DRHI.CreateImage(FontImageDesc);

		unsigned char* Pixels_RT = new unsigned char[m_DefaultFontImage->ByteSize()];
		memcpy(Pixels_RT, pixels, m_DefaultFontImage->ByteSize());
		SharedPtr<RHIImage> FontImage = m_DefaultFontImage;
		RenderThread::Enqueue([FontImage, Pixels_RT](RHIContextImmediate& Context)
			{
				void* Mapped = Context.Map(FontImage, ERHIMapAccess::Write);
				if (Mapped != nullptr)
				{
					memcpy(Mapped, Pixels_RT, FontImage->ByteSize());
					Context.UnMap(FontImage);
				}

				delete[] Pixels_RT;
			});

		// Set Default Font Texture ID
		io.Fonts->SetTexID(m_DefaultFontImage.Get());


		// Set Engine Style
		SetupStyle();
	}

	void ImGuiInterface::SetupStyle()
	{
		ImGuiStyle& Style = ImGui::GetStyle();
		Style.WindowRounding = 0.0f;
		Style.ChildRounding = 0.0f;
		Style.FrameRounding = 0.0f;
		Style.GrabRounding = 0.0f;
		Style.PopupRounding = 0.0f;
		Style.ScrollbarRounding = 0.0f;
		Style.TabRounding = 0.0f;
		Style.WindowMenuButtonPosition = ImGuiDir_Right;

		constexpr auto ColorToImVec4 = [](const Color& Color)
		{
			return ImVec4(Color.R, Color.G, Color.B, Color.A);
		};

		const ImVec4 Vec4_TextColor = ColorToImVec4(ImGuiEditorStyle::TextColor);
		const ImVec4 Vec4_TextColor_Disabled = ColorToImVec4(ImGuiEditorStyle::TextColor_Disabled);
		const ImVec4 Vec4_BackgroundColor = ColorToImVec4(ImGuiEditorStyle::BackgroundColor);
		const ImVec4 Vec4_ScrollbarColor = ColorToImVec4(ImGuiEditorStyle::ScrollBarColor);
		const ImVec4 Vec4_Scrollbar_Active = ColorToImVec4(ImGuiEditorStyle::ScrollBarColor_Active);
		const ImVec4 Vec4_BorderColor = ColorToImVec4(ImGuiEditorStyle::BorderColor);
		const ImVec4 Vec4_AccentColor = ColorToImVec4(ImGuiEditorStyle::AccentColor);
		const ImVec4 Vec4_AccentColor_Hover = ColorToImVec4(ImGuiEditorStyle::AccentColor_Hovered);
		const ImVec4 Vec4_AccentColor_Active = ColorToImVec4(ImGuiEditorStyle::AccentColor_Active);

		Style.Colors[ImGuiCol_Text] = Vec4_TextColor;
		Style.Colors[ImGuiCol_TextDisabled] = Vec4_TextColor_Disabled;
		Style.Colors[ImGuiCol_TextSelectedBg] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_WindowBg] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_ChildBg] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_PopupBg] = ImVec4(Vec4_BackgroundColor.x, Vec4_BackgroundColor.y, Vec4_BackgroundColor.z, 1.0f);
		Style.Colors[ImGuiCol_Border] = Vec4_BorderColor;
		Style.Colors[ImGuiCol_BorderShadow] = Vec4_BorderColor;
		Style.Colors[ImGuiCol_FrameBg] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_FrameBgHovered] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_FrameBgActive] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_TitleBg] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_TitleBgActive] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_TitleBgCollapsed] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_MenuBarBg] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_ScrollbarBg] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_ScrollbarGrab] = Vec4_ScrollbarColor;
		Style.Colors[ImGuiCol_ScrollbarGrabHovered] = Vec4_Scrollbar_Active;
		Style.Colors[ImGuiCol_ScrollbarGrabActive] = Vec4_Scrollbar_Active;
		Style.Colors[ImGuiCol_CheckMark] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_SliderGrab] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_SliderGrabActive] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_Button] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_ButtonHovered] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_ButtonActive] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_Header] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_HeaderHovered] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_HeaderActive] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_Separator] = Vec4_BorderColor;
		Style.Colors[ImGuiCol_SeparatorHovered] = Vec4_BorderColor;
		Style.Colors[ImGuiCol_SeparatorActive] = Vec4_BorderColor;
		Style.Colors[ImGuiCol_ResizeGrip] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_ResizeGripHovered] = Vec4_AccentColor;
		Style.Colors[ImGuiCol_ResizeGripActive] = Vec4_ScrollbarColor;
		Style.Colors[ImGuiCol_PlotLines] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_PlotLinesHovered] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_PlotHistogram] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_PlotHistogramHovered] = Vec4_AccentColor_Hover;
		Style.Colors[ImGuiCol_DragDropTarget] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_NavHighlight] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_DockingPreview] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_Tab] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_TabActive] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_TabUnfocused] = Vec4_BackgroundColor;
		Style.Colors[ImGuiCol_TabUnfocusedActive] = Vec4_AccentColor_Active;
		Style.Colors[ImGuiCol_TabHovered] = Vec4_AccentColor_Hover;
	}

	void ImGuiInterface::BeginFrame(float DeltaTime)
	{
		ImGuiIO& io = ImGui::GetIO();

		SharedPtr<Window> Window = m_Window.Lock();

		io.DeltaTime = DeltaTime;
		Vec2ui DisplaySize = Window != nullptr ? Window->GetSize() : Vec2ui(1, 1);
		io.DisplaySize.x = (float)DisplaySize.X;
		io.DisplaySize.y = (float)DisplaySize.Y;

		io.KeyCtrl = Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Left_Control) || Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Right_Control);
		io.KeyShift = Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Left_Shift) || Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Right_Shift);
		io.KeyAlt = Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Left_Alt) || Engine::Get().Application()->IsInputPressed(EInput::Keyboard_Right_Alt);
		io.KeySuper = false;

		if (io.WantSetMousePos && Window != nullptr)
		{
			Vec2<long> pos = { (int)io.MousePos.x, (int)io.MousePos.y };
			if (!(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
				Engine::Get().Application()->SetCursorPosition(pos, Window.Get());
			else
				Engine::Get().Application()->SetCursorPosition(pos);
		}

		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		io.MouseHoveredViewport = 0;

		// Set imgui mouse position
		CE::Window* FocusedWindow = Engine::Get().Application()->GetForegroundWindow();
		if (FocusedWindow != nullptr)
		{
			//if (::IsChild(focused_hwnd, g_hWnd))	//@Note: We don't allow Child windows
			//	focused_hwnd = g_hWnd;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
				// This is the position you can get with GetCursorPos(). In theory adding viewport->Pos is also the reverse operation of doing ScreenToClient().
				if (ImGui::FindViewportByPlatformHandle((void*)FocusedWindow) != NULL)
				{
					Vec2<long> mouse_screen_pos = Engine::Get().Application()->GetCursorPosition();
					io.MousePos = ImVec2((float)mouse_screen_pos.X, (float)mouse_screen_pos.Y);
				}
			}
			else
			{
				// Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window.)
				// This is the position you can get with GetCursorPos() + ScreenToClient() or from WM_MOUSEMOVE.
				if (FocusedWindow != nullptr && FocusedWindow == Window.Get())
				{
					Vec2<long> mouse_client_pos = Engine::Get().Application()->GetCursorPosition(Window.Get());
					io.MousePos = ImVec2((float)mouse_client_pos.X, (float)mouse_client_pos.Y);
				}
			}
		}

		//@TODO: Implement if Viewports are implemented
		// (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
		// Important: this information is not easy to provide and many high-level windowing library won't be able to provide it correctly, because
		// - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
		// - This is _regardless_ of whether another viewport is focused or being dragged from.
		// If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, imgui will ignore this field and infer the information by relying on the
		// rectangles and last focused time of every viewports it knows about. It will be unaware of foreign windows that may be sitting between or over your windows.
		if (CE::Window* HoveredWindow = Engine::Get().Application()->GetWindowFromPoint(Vec2f(io.MousePos.x, io.MousePos.y)))
		{
			if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)HoveredWindow))
			{
				if ((viewport->Flags & ImGuiViewportFlags_NoInputs) == 0) // FIXME: We still get our NoInputs window with WM_NCHITTEST/HTTRANSPARENT code when decorated?
				{
					io.MouseHoveredViewport = viewport->ID;
				}
			}
		}


		static ImGuiMouseCursor LastCursor = ImGuiMouseCursor_Arrow;
		ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (LastCursor != mouse_cursor)
		{
			if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
			{
				LastCursor = mouse_cursor;
				ECursor NewCursor = ECursor::Arrow;
				switch (mouse_cursor)
				{
				case ImGuiMouseCursor_None:			NewCursor = ECursor::None; break;
				case ImGuiMouseCursor_Arrow:        NewCursor = ECursor::Arrow; break;
				case ImGuiMouseCursor_TextInput:    NewCursor = ECursor::TextInput; break;
				case ImGuiMouseCursor_ResizeAll:    NewCursor = ECursor::Resize_All; break;
				case ImGuiMouseCursor_ResizeEW:     NewCursor = ECursor::Resize_EW; break;
				case ImGuiMouseCursor_ResizeNS:     NewCursor = ECursor::Resize_NS; break;
				case ImGuiMouseCursor_ResizeNESW:   NewCursor = ECursor::Resize_NESW; break;
				case ImGuiMouseCursor_ResizeNWSE:   NewCursor = ECursor::Resize_NWSE; break;
				case ImGuiMouseCursor_Hand:         NewCursor = ECursor::Hand; break;
				case ImGuiMouseCursor_NotAllowed:   NewCursor = ECursor::NotAllowed; break;
				}
				if (Window != nullptr)
				{
					Window->SetCursor(NewCursor);
				}
			}
		}

		ImGui::NewFrame();
	}

	void ImGuiInterface::EndFrame()
	{
		ImGui::Render();

		RenderImGuiViewport(ImGui::GetMainViewport());

		OneTimeImages.Clear();
	}

	void ImGuiInterface::Terminate()
	{
		// Reset the Main Viewport
		ImGuiViewport* MainViewport = ImGui::GetMainViewport();
		MainViewport->PlatformHandle = MainViewport->PlatformHandleRaw = nullptr;

		// Destroy Main Viewport Buffers
		ImGuiDrawBuffers* DrawBuffers = (ImGuiDrawBuffers*)MainViewport->RendererUserData;
		delete(DrawBuffers);
		MainViewport->RendererUserData = nullptr;

		// Destroy All Platform Windows
		ImGui::DestroyPlatformWindows();

		// Reset RHI Objects
		//m_DefaultFontImage.Reset();
		m_Sampler.Reset();
		m_Pipeline.Reset();

		// Destroy ImGui Context
		ImGui::DestroyContext();

		m_DefaultFontImage.Reset();
	}

	void ImGuiInterface::SetWindow(const SharedPtr<Window>& NewWindow)
	{
		m_Window = NewWindow;

		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->PlatformHandle = main_viewport->PlatformHandleRaw = (void*)NewWindow.Get();
	}

	SharedPtr<Window> ImGuiInterface::GetWindow() const
	{
		return m_Window.Lock();
	}

	void ImGuiInterface::OnKeyboardKey(Window* Window, EInput Key, bool Pressed, EInputModifier Modifier)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[(int)Key] = Pressed;
	}

	void ImGuiInterface::OnMouseButton(Window* Window, EInput Button, bool Pressed, EInputModifier Modifier)
	{
		ImGuiIO& io = ImGui::GetIO();

		//@TODO: This is necessary if we leave the window while Mouse is down it doesn't get release again
		//if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
		//	::SetCapture(hwnd);
		int ImGuiButton = -1;
		switch (Button)
		{
		case EInput::Mouse_Left:
			ImGuiButton = 0;
			break;
		case EInput::Mouse_Middle:
			ImGuiButton = 1;
			break;
		case EInput::Mouse_Right:
			ImGuiButton = 2;
			break;
		case EInput::Mouse_4:
			ImGuiButton = 3;
			break;
		case EInput::Mouse_5:
			ImGuiButton = 4;
			break;
		}
		if (ImGuiButton != -1)
		{
			io.MouseDown[ImGuiButton] = Pressed;
		}
	}

	void ImGuiInterface::OnMouseScroll(Window* Window, double ScrollVertical, double ScrollHorizontal)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += (float)ScrollVertical;
		io.MouseWheelH += (float)ScrollHorizontal;
	}

	void ImGuiInterface::OnChar(Window* Window, unsigned short Char)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacterUTF16(Char);
	}

	const CE::String& ImGuiInterface::GetImGuiShaderSource()
	{
		static String ShaderCode(R"-(struct VS_INPUT
		{
			float2 pos : ATTRIB0;
			float2 uv : ATTRIB1;
			float4 col : ATTRIB2;
		};

		struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float4 col : COLOR0;
			float2 uv : TEXCOORD0;
		};

		cbuffer VertexBuffer : register(b0)
		{
			float4x4 ProjectionMatrix;
		};

		PS_INPUT VShader(VS_INPUT input)
		{
			PS_INPUT output;
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
            output.col = input.col;
            output.uv  = input.uv;
            return output;
		}

		Texture2D texture0 : register(t1);
		sampler sampler0 : register(s2);
		
		float4 FShader(PS_INPUT input) : SV_Target
		{
			float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
			return out_col;
		}
		)-");
		return ShaderCode;
	}

	void ImGuiInterface::RenderImGuiViewport(ImGuiViewport* Viewport)
	{
		DynamicRHI& DRHI = Engine::Get().DynamicRHI();

		Window* Win = (Window*)Viewport->PlatformHandle;
		if (Win == nullptr)
		{
			return;
		}

		SharedPtr<Swapchain> WinSwapchain = Win->GetSwapchain();
		ImDrawData* DrawData = Viewport->DrawData;

		if (Win != nullptr && WinSwapchain != nullptr && DrawData != nullptr && DrawData->TotalVtxCount > 0)
		{
			int fb_width = (int)(DrawData->DisplaySize.x * DrawData->FramebufferScale.x);
			int fb_height = (int)(DrawData->DisplaySize.y * DrawData->FramebufferScale.y);

			if (fb_width > 0 && fb_height > 0)
			{
				struct RenderCmdList
				{
					Array<ImDrawCmd> Commands;
					uint32 VertexCount = 0;
					uint32 IndexCount = 0;
				};
				struct DataRequiredForRenderThread
				{
					Vec2f DisplaySize;
					Vec2i FBSize;
					Vec2f ClipOff;
					Vec2f ClipScale;
					Mat4x4f MVP;

					Array<RenderCmdList> CommandLists;
					Array<ImDrawVert> Vertices;
					Array<ImDrawIdx> Indices;

					SharedPtr<Swapchain> Swapchain;
					SharedPtr<RHIBuffer> VertexBuffer;
					SharedPtr<RHIBuffer> IndexBuffer;
					SharedPtr<RHIBuffer> UniformBuffer;
					SharedPtr<Sampler> Sampler;
					SharedPtr<RHIImage> FontImage;
					SharedPtr<RHIPipeline_Graphic> Pipeline;
					Array<SharedPtr<RHIImage>> TemporaryImages;
				};

				ImGuiDrawBuffers* DrawBuffers = (ImGuiDrawBuffers*)Viewport->RendererUserData;
				if (DrawBuffers == nullptr)
				{
					Viewport->RendererUserData = DrawBuffers = new ImGuiDrawBuffers();

					RHIBufferDescription UBufferDesc;
					UBufferDesc.Size = sizeof(Mat4x4f);
					UBufferDesc.ElementSize = sizeof(Mat4x4f);
					UBufferDesc.Usage = ERHIBufferUsage::UniformBuffer;
					UBufferDesc.Access = ERHIAccess::GPURead_CPUWrite;
					DrawBuffers->UniformBuffer = DRHI.CreateBuffer(UBufferDesc);
				}

				if (DrawBuffers->VertexBuffer == nullptr || DrawBuffers->VertexBuffer->Description.Size != DrawData->TotalVtxCount * sizeof(ImDrawVert))
				{
					RHIBufferDescription UBufferDesc;
					UBufferDesc.Size = sizeof(ImDrawVert) * DrawData->TotalVtxCount;
					UBufferDesc.ElementSize = sizeof(ImDrawVert);
					UBufferDesc.Usage = ERHIBufferUsage::VertexBuffer;
					UBufferDesc.Access = ERHIAccess::GPURead_CPUWrite;
					DrawBuffers->VertexBuffer = DRHI.CreateBuffer(UBufferDesc);
				}

				if (DrawBuffers->IndexBuffer == nullptr || DrawBuffers->IndexBuffer->Description.Size != DrawData->TotalIdxCount * sizeof(ImDrawVert))
				{
					RHIBufferDescription UBufferDesc;
					UBufferDesc.Size = sizeof(ImDrawIdx) * DrawData->TotalIdxCount;
					UBufferDesc.ElementSize = sizeof(ImDrawIdx);
					UBufferDesc.Usage = ERHIBufferUsage::IndexBuffer;
					UBufferDesc.Access = ERHIAccess::GPURead_CPUWrite;
					DrawBuffers->IndexBuffer = DRHI.CreateBuffer(UBufferDesc);
				}


				//@TODO: If the Task will never be executed this will not be deleted and will leak
				DataRequiredForRenderThread* DRFRT = new DataRequiredForRenderThread();
				DRFRT->DisplaySize = Vec2f(DrawData->DisplaySize.x, DrawData->DisplaySize.y);
				DRFRT->FBSize = Vec2i(fb_width, fb_height);
				DRFRT->ClipOff = Vec2f(DrawData->DisplayPos.x, DrawData->DisplayPos.y);         // (0,0) unless using multi-viewports
				DRFRT->ClipScale = Vec2f(DrawData->FramebufferScale.x, DrawData->FramebufferScale.y); // (1,1) unless using retina display which are often (2,2)

				float L = DrawData->DisplayPos.x;
				float R = DrawData->DisplayPos.x + DrawData->DisplaySize.x;
				float T = DrawData->DisplayPos.y;
				float B = DrawData->DisplayPos.y + DrawData->DisplaySize.y;
				DRFRT->MVP = Mat4x4f(
					Vec4f(2.0f / (R - L), 0.0f, 0.0f, 0.0f),
					Vec4f(0.0f, 2.0f / (T - B), 0.0f, 0.0f),
					Vec4f(0.0f, 0.0f, 0.5f, 0.0f),
					Vec4f((R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f)
				);

				DRFRT->Swapchain = WinSwapchain;
				DRFRT->VertexBuffer = DrawBuffers->VertexBuffer;
				DRFRT->IndexBuffer = DrawBuffers->IndexBuffer;
				DRFRT->UniformBuffer = DrawBuffers->UniformBuffer;
				DRFRT->Sampler = m_Sampler;
				DRFRT->FontImage = m_DefaultFontImage;
				DRFRT->Pipeline = m_Pipeline;
				DRFRT->TemporaryImages = OneTimeImages;

				DRFRT->Vertices.AddUninitialized(DrawData->TotalVtxCount);
				DRFRT->Indices.AddUninitialized(DrawData->TotalIdxCount);

				ImDrawVert* vtx_dst = (ImDrawVert*)DRFRT->Vertices.Data();
				ImDrawIdx* idx_dst = (ImDrawIdx*)DRFRT->Indices.Data();
				for (int n = 0; n < DrawData->CmdListsCount; n++)
				{
					const ImDrawList* cmd_list = DrawData->CmdLists[n];

					RenderCmdList& CmdList = DRFRT->CommandLists.EmplaceRef();
					CmdList.VertexCount = cmd_list->VtxBuffer.Size;
					CmdList.IndexCount = cmd_list->IdxBuffer.Size;
					CmdList.Commands.AddUninitialized(cmd_list->CmdBuffer.Size);
					memcpy(CmdList.Commands.Data(), cmd_list->CmdBuffer.Data, cmd_list->CmdBuffer.Size * sizeof(ImDrawCmd));

					memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
					memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
					vtx_dst += cmd_list->VtxBuffer.Size;
					idx_dst += cmd_list->IdxBuffer.Size;
				}

				RenderThread::Enqueue([DRFRT](RHIContextImmediate& Context)
					{
						SharedPtr<RHIImage> RT = DRFRT->Swapchain->CurrentImage();

						void* VMapped = Context.Map(DRFRT->VertexBuffer, ERHIMapAccess::Write);
						if (VMapped != nullptr)
						{
							memcpy(VMapped, DRFRT->Vertices.Data(), sizeof(ImDrawVert) * DRFRT->Vertices.Size());
							Context.UnMap(DRFRT->VertexBuffer);
						}

						void* IMapped = Context.Map(DRFRT->IndexBuffer, ERHIMapAccess::Write);
						if (IMapped != nullptr)
						{
							memcpy(IMapped, DRFRT->Indices.Data(), sizeof(ImDrawIdx) * DRFRT->Indices.Size());
							Context.UnMap(DRFRT->IndexBuffer);
						}

						void* UMapped = Context.Map(DRFRT->UniformBuffer, ERHIMapAccess::Write);
						if (UMapped != nullptr)
						{
							memcpy(UMapped, &DRFRT->MVP, sizeof(DRFRT->MVP));
							Context.UnMap(DRFRT->UniformBuffer);
						}

						ViewportArea Viewport;
						Viewport.Width = DRFRT->DisplaySize.X;
						Viewport.Height = DRFRT->DisplaySize.Y;
						uint32 Offset = 0;

						Context.BeginRenderpass({ RT }, nullptr);

						Context.BindGraphicPipeline(DRFRT->Pipeline);
						Context.SetViewports(1, &Viewport);
						Context.BindVertexBuffers(1, &DRFRT->VertexBuffer, &Offset);
						Context.BindIndexBuffer(DRFRT->IndexBuffer, sizeof(ImDrawIdx) == sizeof(uint32) ? EIndexBufferType::UInt32 : EIndexBufferType::UInt16);
						Context.BindShaderResource(0, DRFRT->UniformBuffer);
						Context.BindShaderResource(1, DRFRT->FontImage);
						Context.BindShaderResource(2, DRFRT->Sampler);

						int global_idx_offset = 0;
						int global_vtx_offset = 0;
						for (auto& CmdList : DRFRT->CommandLists)
						{
							for (auto& Cmd : CmdList.Commands)
							{
								if (Cmd.UserCallback != NULL)
								{
									// User callback, registered via ImDrawList::AddCallback()
									// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
									if (Cmd.UserCallback == ImDrawCallback_ResetRenderState)
									{
										Context.BindGraphicPipeline(DRFRT->Pipeline);
										Context.SetViewports(1, &Viewport);
										Context.BindVertexBuffers(1, &DRFRT->VertexBuffer, &Offset);
										Context.BindIndexBuffer(DRFRT->IndexBuffer, sizeof(ImDrawIdx) == sizeof(uint32) ? EIndexBufferType::UInt32 : EIndexBufferType::UInt16);
										Context.BindShaderResource(0, DRFRT->UniformBuffer);
										Context.BindShaderResource(1, DRFRT->FontImage);
										Context.BindShaderResource(2, DRFRT->Sampler);
									}
									else
									{
										CE_ASSERT(false, "Usercallbacks are unsupported");
										//Cmd.UserCallback(CmdList, &Cmd);
									}
								}
								else
								{
									// Project scissor/clipping rectangles into framebuffer space
									ImVec4 clip_rect;
									clip_rect.x = (Cmd.ClipRect.x - DRFRT->ClipOff.X) * DRFRT->ClipScale.X;
									clip_rect.y = (Cmd.ClipRect.y - DRFRT->ClipOff.Y) * DRFRT->ClipScale.Y;
									clip_rect.z = (Cmd.ClipRect.z - DRFRT->ClipOff.X) * DRFRT->ClipScale.X;
									clip_rect.w = (Cmd.ClipRect.w - DRFRT->ClipOff.Y) * DRFRT->ClipScale.Y;

									if (clip_rect.x < DRFRT->FBSize.X && clip_rect.y < DRFRT->FBSize.Y && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
									{
										// Negative offsets are illegal for vkCmdSetScissor
										if (clip_rect.x < 0.0f)
											clip_rect.x = 0.0f;
										if (clip_rect.y < 0.0f)
											clip_rect.y = 0.0f;

										// Apply scissor/clipping rectangle
										ScissorArea Scissor;
										Scissor.OffsetX = (int32_t)(clip_rect.x);
										Scissor.OffsetY = (int32_t)(clip_rect.y);
										Scissor.ExtentX = (int32_t)(clip_rect.z - clip_rect.x);
										Scissor.ExtentY = (int32_t)(clip_rect.w - clip_rect.y);
										Context.SetScissors(1, &Scissor);

										RHIImage* Handle = reinterpret_cast<RHIImage*>(Cmd.GetTexID());
										Context.BindShaderResource(1, SharedPtr<RHIImage>::DynamicCast(Handle->GetShared()));
										Context.DrawIndexedPrimitive(Cmd.ElemCount, Cmd.IdxOffset + global_idx_offset, Cmd.VtxOffset + global_vtx_offset);
									}
								}
							}
							global_vtx_offset += CmdList.VertexCount;
							global_idx_offset += CmdList.IndexCount;
						}

						Context.EndRenderpass();

						delete(DRFRT);
					});
			}
		}
	}
}
