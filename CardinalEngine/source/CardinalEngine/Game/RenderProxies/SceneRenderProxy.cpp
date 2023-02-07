#include "CardinalEngine/Game/RenderProxies/SceneRenderProxy.h"
#include "CardinalEngine/Engine/Engine.h"

#include "CardinalEngine/Rendering/RenderTarget.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Rendering/RHI/Pipeline.h"
#include "CardinalEngine/Rendering/ShaderCompiler.h"

#include "CardinalEngine/Game/RenderProxies/RenderProxy.h"
#include "CardinalEngine/Game/RenderProxies/RP_Camera.h"
#include "CardinalEngine/Game/RenderProxies/RP_Geometry.h"
#include "CardinalEngine/Game/RenderProxies/RP_Light.h"

#include "CardinalEngine/Mesh/MeshVertex.h"
#include "CardinalEngine/Material/Material.h"

namespace CE
{
	SceneRenderProxy::SceneRenderProxy()
	{
		MaterialSettings MatSettings;
		//MatSettings.Shading = EMaterialShading::Unlit;
		m_DefaultMaterial = SharedPtr<Material>(new Material(MatSettings));
	}

	void SceneRenderProxy::AddProxy(const SharedPtr<RenderProxy>& Proxy)
	{
		SharedPtr<RP_Camera> CameraProxy = SharedPtr<RP_Camera>::DynamicCast(Proxy);
		if (CameraProxy != nullptr)
		{
			m_CameraRenderProxies.Emplace(CameraProxy);
		}

		SharedPtr<RP_Geometry> GemotryProxy = SharedPtr<RP_Geometry>::DynamicCast(Proxy);
		if (GemotryProxy != nullptr)
		{
			m_GeometryRenderProxies.Emplace(GemotryProxy);
		}

		SharedPtr<RP_Light> LightProxy = SharedPtr<RP_Light>::DynamicCast(Proxy);
		if (LightProxy != nullptr)
		{
			m_LightRenderProxies.Emplace(LightProxy);
		}
	}

	void SceneRenderProxy::RemoveProxy(const SharedPtr<RenderProxy>& Proxy)
	{
		SharedPtr<RP_Camera> CameraProxy = SharedPtr<RP_Camera>::DynamicCast(Proxy);
		if (CameraProxy != nullptr)
		{
			m_CameraRenderProxies.Remove(CameraProxy);
		}

		SharedPtr<RP_Geometry> GemotryProxy = SharedPtr<RP_Geometry>::DynamicCast(Proxy);
		if (GemotryProxy != nullptr)
		{
			m_GeometryRenderProxies.Remove(GemotryProxy);
		}

		SharedPtr<RP_Light> LightProxy = SharedPtr<RP_Light>::DynamicCast(Proxy);
		if (LightProxy != nullptr)
		{
			m_LightRenderProxies.Remove(LightProxy);
		}
	}

	void SceneRenderProxy::RenderScene(RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{
		for (auto& Camera : m_CameraRenderProxies)
		{
			RP_Camera& Cam = *Camera.Get();

			SharedPtr<RenderTarget> Target = Camera->GetRenderTarget();
			if (Target == nullptr || Target->GetTargetImage() == nullptr)
			{
				continue;
			}

			ClearPass(Cam, Context, RenderData);

			ShadowMapPass(Cam, Context, RenderData);
			PreDepthPass(Cam, Context, RenderData);
			HierarchicalZBufferCompute(Cam, Context, RenderData);
			PreIntegration(Cam, Context, RenderData);
			LightCullingPass(Cam, Context, RenderData);

			GeometryPass(Cam, Context, RenderData);
			DeinterleavingPass(Cam, Context, RenderData);
			HBAOCompute(Cam, Context, RenderData);
			ReinterleavingPass(Cam, Context, RenderData);
			HBAOBlurPass(Cam, Context, RenderData);

			GTAOPass(Cam, Context, RenderData);
			GTAODenoiseCompute(Cam, Context, RenderData);

			HBAOAndGTAOCompositePass(Cam, Context, RenderData);

			PostProcessingPass(Cam, Context, RenderData);

			SSRCompute(Cam, Context, RenderData);
			SSRCompositePass(Cam, Context, RenderData);

			BloomCompute(Cam, Context, RenderData);

			CompositePass(Cam, Context, RenderData);
		}
	}

	void SceneRenderProxy::ClearPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{
		SharedPtr<RenderTarget> Target = Camera.GetRenderTarget();
		SharedPtr<RHIImage> DepthTarget = Camera.GetDepthTarget();
		Context.BeginRenderpass({ Target->GetTargetImage() }, DepthTarget);

		Context.ClearRenderTarget(Target->GetTargetImage(), Color::FromRGB(0.2f, 0.2f, 0.2f));
		if (DepthTarget != nullptr)
		{
			Context.ClearDepthTarget(DepthTarget, true, 1.0f, true, 1);
		}

		Context.EndRenderpass();
	}

	void SceneRenderProxy::ShadowMapPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::PreDepthPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{
		SharedPtr<RHIImage> DepthTarget = Camera.GetDepthTarget();
		Context.BeginRenderpass({}, DepthTarget);

		ViewportArea Viewport;
		Viewport.Width = DepthTarget->Description.Width;
		Viewport.Height = DepthTarget->Description.Height;
		Context.SetViewports(1, &Viewport);

		ScissorArea Scissor;
		Scissor.ExtentX = Viewport.Width;
		Scissor.ExtentY = Viewport.Height;
		Context.SetScissors(1, &Scissor);

		DrawGeometry(Camera, Context, nullptr, true);

		Context.EndRenderpass();
	}

	void SceneRenderProxy::HierarchicalZBufferCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::PreIntegration(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::LightCullingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::GeometryPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{
		SharedPtr<RenderTarget> Target = Camera.GetRenderTarget();
		SharedPtr<RHIImage> DepthTarget = Camera.GetDepthTarget();
		Context.BeginRenderpass({ Target->GetTargetImage() }, DepthTarget);

		ViewportArea Viewport;
		Viewport.Width = Target->GetTargetImage()->Description.Width;
		Viewport.Height = Target->GetTargetImage()->Description.Height;
		Context.SetViewports(1, &Viewport);

		ScissorArea Scissor;
		Scissor.ExtentX = Viewport.Width;
		Scissor.ExtentY = Viewport.Height;
		Context.SetScissors(1, &Scissor);

		DrawGeometry(Camera, Context, nullptr, false);

		Context.EndRenderpass();
	}

	void SceneRenderProxy::DeinterleavingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::HBAOCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::ReinterleavingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::HBAOBlurPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::GTAOPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::GTAODenoiseCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::HBAOAndGTAOCompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::PostProcessingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::SSRCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::SSRCompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::BloomCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::CompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData)
	{

	}

	void SceneRenderProxy::DrawGeometry(RP_Camera& Camera, RHIContextImmediate& Context, SharedPtr<Material> MaterialOverride, bool UseDepth)
	{
		for (auto& Geometry : m_GeometryRenderProxies)
		{
			SharedPtr<Material> UsedMaterial = MaterialOverride;

			AssetRef<Material> Mat = Geometry->GetMaterial();
			if (Mat.State() == EAssetState::Ready)
			{
				UsedMaterial = Mat.Get();
			}

			if (UsedMaterial == nullptr)
			{
				UsedMaterial = m_DefaultMaterial;
			}

			Context.BindGraphicPipeline(UseDepth ? UsedMaterial->DepthPipeline() : UsedMaterial->PBRPipeline());

			for (auto& SamplerIt : UsedMaterial->Samplers())
			{
				Context.BindShaderResource(SamplerIt.second.Binding, SamplerIt.second.Sampler);
			}

			for (auto& TextureIt : UsedMaterial->Textures())
			{
				SharedPtr<Texture> Texture = TextureIt.second.Texture.Get();
				Context.BindShaderResource(TextureIt.second.Binding, Texture->Image());
			}

			if (UsedMaterial->BufferBinding(EMaterialBufferType::Frame) != -1)
			{
				//Context.BindShaderResource(UsedMaterial->BufferBinding(EMaterialBufferType::Frame), nullptr);
			}

			if (UsedMaterial->BufferBinding(EMaterialBufferType::Environment) != -1)
			{
				//Context.BindShaderResource(UsedMaterial->BufferBinding(EMaterialBufferType::Environment), nullptr);
			}

			if (UsedMaterial->BufferBinding(EMaterialBufferType::Camera) != -1)
			{
				Context.BindShaderResource(UsedMaterial->BufferBinding(EMaterialBufferType::Camera), Camera.GetProjectionViewBuffer());
			}

			if (UsedMaterial->BufferBinding(EMaterialBufferType::DynamicParameters) != -1)
			{
				Context.BindShaderResource(UsedMaterial->BufferBinding(EMaterialBufferType::DynamicParameters), UsedMaterial->DynamicParameterBuffer());
			}

			Geometry->DrawGeometry(Context);
		}
	}
}
