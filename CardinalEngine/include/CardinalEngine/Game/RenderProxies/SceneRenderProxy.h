#pragma once
#include "../../Framework/Core/Headers.h"

namespace CE
{
	class RenderProxy;
	class RP_Camera;
	class RP_Light;
	class RP_Geometry;
	class RHIContextImmediate;
	class Material;
	class Texture;
	class Sampler;
	class RHIPipeline_Graphic;
	class RHIBuffer;

	struct SceneRenderData 
	{
		SharedPtr<RHIBuffer> FrameBuffer;
		SharedPtr<RHIBuffer> EnvironmentBuffer;
	};

	class CE_API SceneRenderProxy : public Singleton
	{
	public:
		SceneRenderProxy();

		void AddProxy(const SharedPtr<RenderProxy>& Proxy);

		void RemoveProxy(const SharedPtr<RenderProxy>& Proxy);

		void RenderScene(RHIContextImmediate& Context, const SceneRenderData& RenderData);

	private:
		void ClearPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);

		void ShadowMapPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void PreDepthPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void HierarchicalZBufferCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void PreIntegration(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void LightCullingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void GeometryPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void DeinterleavingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void HBAOCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void ReinterleavingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void HBAOBlurPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void GTAOPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void GTAODenoiseCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void HBAOAndGTAOCompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void PostProcessingPass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void SSRCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		void SSRCompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void BloomCompute(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);
		
		void CompositePass(RP_Camera& Camera, RHIContextImmediate& Context, const SceneRenderData& RenderData);



		void DrawGeometry(RP_Camera& Camera, RHIContextImmediate& Context, SharedPtr<Material> MaterialOverride, bool UseDepth);

	private:
		SharedPtr<Material> m_DefaultMaterial;
		//SharedPtr<Texture> m_DefaultTexture;
		//SharedPtr<Sampler> m_DefaultSampler;

		Array<SharedPtr<RP_Camera>> m_CameraRenderProxies;
		Array<SharedPtr<RP_Geometry>> m_GeometryRenderProxies;
		Array<SharedPtr<RP_Light>> m_LightRenderProxies;
	};
}
