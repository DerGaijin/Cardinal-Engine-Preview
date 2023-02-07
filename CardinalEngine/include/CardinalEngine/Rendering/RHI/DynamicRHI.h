#pragma once
#include "../../Framework/Core/Headers.h"
#include "Enums.h"

namespace CE
{
	class Window;
	class Swapchain;
	class RHIContextImmediate;
	class RHIImage;
	struct RHIImageDescription;
	class RHIBuffer;
	struct RHIBufferDescription;
	class VertexShader;
	class TessellationControlShader;
	class TessellationEvaluationShader;
	class GeometryShader;
	class FragmentShader;
	class ComputeShader;
	struct RHIPipeline_GraphicShaders;
	struct RHIPipeline_GraphicDescription;
	class RHIPipeline_Graphic;
	class Sampler;
	struct SamplerDescription;

	struct RHICapabilities 
	{
		Text DeviceName;
	};

	struct RHIConfig 
	{
		uint32 FramesInFlight = 2;
		EDataFormat SwapchainImageFormat = EDataFormat::RGBA8_UNorm;
		// SampleCount
	};

	class CE_API DynamicRHI : public Singleton
	{
	public:
		static uint32 SizeOfDataFormat(EDataFormat Format);

		static bool IsRHIAvailable(ERHIType Type);

		static ERHIType ChooseRHIType();

		static DynamicRHI* Create(ERHIType Type);

		template<typename T>
		static SharedPtr<T> CreateRHIPtr(T* Obj)
		{
			return SharedPtr<T>::Create(Obj, RHIObjectDeleter<T>());
		}

	public:
		virtual bool Load() = 0;

		virtual bool Initialize(const RHIConfig& Config);

		virtual ERHIType Type() const
		{
			return ERHIType::None;
		}

		virtual void Tick(float DeltaTime) = 0;

		virtual void Terminate();
	
		virtual void Unload() = 0;

	public:
		virtual SharedPtr<Swapchain> CreateSwapchain(Window* Window) = 0;

		virtual SharedPtr<RHIImage> CreateImage(const RHIImageDescription& Description) = 0;

		virtual SharedPtr<RHIBuffer> CreateBuffer(const RHIBufferDescription& Description) = 0;

		virtual SharedPtr<VertexShader> CreateVertexShader(const char* Data, size_t DataSize, const char* Entry) = 0;
		virtual SharedPtr<TessellationControlShader> CreateTessellationControlShader(const char* Data, size_t DataSize, const char* Entry) = 0;
		virtual SharedPtr<TessellationEvaluationShader> CreateTessellationEvaluationShader(const char* Data, size_t DataSize, const char* Entry) = 0;
		virtual SharedPtr<GeometryShader> CreateGeometryShader(const char* Data, size_t DataSize, const char* Entry) = 0;
		virtual SharedPtr<FragmentShader> CreateFragmentShader(const char* Data, size_t DataSize, const char* Entry) = 0;
		virtual SharedPtr<ComputeShader> CreateComputeShader(const char* Data, size_t DataSize, const char* Entry) = 0;

		virtual SharedPtr<RHIPipeline_Graphic> CreatePipelineGraphic(const RHIPipeline_GraphicShaders& Shaders, const RHIPipeline_GraphicDescription& Description) = 0;

		virtual SharedPtr<Sampler> CreateSampler(const SamplerDescription& Description) = 0;

	public:
		const RHIConfig& Config() const
		{
			return m_Config;
		}

		const RHICapabilities& Capabilities() const
		{
			return m_Capabilities;
		}

	protected:
		friend class RenderTask;
		virtual RHIContextImmediate& Immediate() const = 0;

		static void SetWindowSwapchain(Window* Window, const SharedPtr<Swapchain>& Swapchain);

	private:
		static bool IsLibraryAvailable(const wchar_t* Lib);


	protected:
		RHIConfig m_Config;
		RHICapabilities m_Capabilities;
	};
}
