#include "Renderer.h"
#include "../render/singletones/ShadowManager.h"
#include "../render/singletones/ParticleSystem.h"
#include "../render/singletones/MeshSystem.h"
#include "../render/singletones/VegetationSystem.h"
#include "../render/singletones/DepthStencilManager.h"
#include "../render/singletones/DecalSystem.h"
#include "../render/singletones/RasteriserStateManager.h"

static float blendFactor[4] = { 0.f,0.f,0.f,0.f };
static UINT sampleMask = 0xffffffff;

namespace engine::windows
{
	ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };

	void Renderer::Init(UINT sampleCount)
	{
		D3D11_INPUT_ELEMENT_DESC input_pl[] =
		{
			{"VERTEX_POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"VIEWPROJ_RX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_RY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_RZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_RW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"VIEWPROJ_LX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_LY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_LZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_LW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"VIEWPROJ_UX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_UY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_UZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_UW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"VIEWPROJ_DX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_DY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_DZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_DW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"VIEWPROJ_FX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_FY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_FZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_FW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"VIEWPROJ_BX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_BY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_BZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_BW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_pl_deferredShadingShader.Init(L"engine/shaders/deferred_pl_shading.hlsl", input_pl, 25, ShaderEnabling(true, false));
		m_pl_grass_deferredShadingShader.Init(L"engine/shaders/deferred_pl_grass_shading.hlsl", input_pl, 25, ShaderEnabling(true, false));

		D3D11_INPUT_ELEMENT_DESC input_dl[] =
		{
			{"VIEWPROJ_X", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_Y", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_Z", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"VIEWPROJ_W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};
		
		m_dl_deferredShadingShader.Init(L"engine/shaders/deferred_dl_shading.hlsl", input_dl, 4, ShaderEnabling(true, false));
		m_env_deferredShadingShader.Init(L"engine/shaders/deferred_env_shading.hlsl", nullptr, 0, ShaderEnabling(true, false));

		m_dl_grass_deferredShadingShader.Init(L"engine/shaders/deferred_dl_grass_shading.hlsl", input_dl, 4, ShaderEnabling(true, false));
		m_env_grass_deferredShadingShader.Init(L"engine/shaders/deferred_env_grass_shading.hlsl", nullptr, 0, ShaderEnabling(true, false));

		m_defDepthStencilState = DepthStencilManager::instance().GetDepthStencilState("default");
		m_defDepthStencilWriteState = DepthStencilManager::instance().GetDepthStencilState("defDepthStencilWrite");
		m_disDepthStencilReadState = DepthStencilManager::instance().GetDepthStencilState("disDepthStencilRead");
		m_defDepthStencilReadState = DepthStencilManager::instance().GetDepthStencilState("defDepthStencilRead");
		m_disableDSS = DepthStencilManager::instance().GetDepthStencilState("disable");

		m_additiveBlendState = BlendStateManager::instance().GetBlendState("additive");
		m_plRasterizerState = RasteriserStateManager::instance().GetRasteriserState("point_light");

		m_sampleCount = sampleCount;
		m_textureRegisterIndex = sampleCount == 1 ? 0 : 1;

		m_hdrRenderTarget.InitFloat16RTV(8, 8, sampleCount);
		m_depthStencil.Init(8, 8, sampleCount);

		m_normalsRTV.InitSnorm16RTV(8, 8, sampleCount);
		m_albedoRTV.InitUnorm8RTV(8, 8, sampleCount);
		m_rmfRTV.InitUnorm8RTV(8, 8, sampleCount);
		m_emissionRTV.InitFloat16RTV(8, 8, sampleCount);
		m_modelIDRTV.InitR16UintRTV(8, 8, sampleCount);

		need_to_resize_RTV = true;
	}

	void Renderer::Render(RenderTargetView& windowRenderTarget, const Camera& camera, PostProcess& postProcess,
		const std::chrono::steady_clock::time_point &currentTime, float deltaTime)
	{
		if (need_to_resize_RTV)
		{
			ResizeRTV(windowRenderTarget);
			need_to_resize_RTV = false;
		}
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();

		engine::Globals::instance().UpdatePerFrameBuffer(camera, currentTime, m_sampleCount, width, height);
		MeshSystem::instance().updateInstances();
		LightSystem::instance().update(camera);
		DecalSystem::instance().update();

		engine::Globals::instance().Bind();

		RenderShadows();

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, width, height);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		RenderToGBuffer();

		DeferredShading();

		ForwardShading(camera, currentTime, deltaTime);

		Resolve(windowRenderTarget, postProcess);
	}

	void Renderer::ResizeRTV(RenderTargetView& windowRenderTarget)
	{
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();
		m_hdrRenderTarget.Resize(width, height);
		m_depthStencil.Resize(width, height);

		m_normalsRTV.Resize(width, height);
		m_albedoRTV.Resize(width, height);
		m_rmfRTV.Resize(width, height);
		m_emissionRTV.Resize(width, height);
		m_modelIDRTV.Resize(width, height);

		m_GBuffer[0] = m_normalsRTV.GetRTV().ptr();
		m_GBuffer[1] = m_albedoRTV.GetRTV().ptr();
		m_GBuffer[2] = m_rmfRTV.GetRTV().ptr();
		m_GBuffer[3] = m_emissionRTV.GetRTV().ptr();
		m_GBuffer[4] = m_modelIDRTV.GetRTV().ptr();

		m_normalsTexture.PointToResource(m_normalsRTV.GetResource(), m_normalsRTV.GetResourceDesc());
		m_albedoTexture.PointToResource(m_albedoRTV.GetResource(), m_albedoRTV.GetResourceDesc());
		m_rmfTexture.PointToResource(m_rmfRTV.GetResource(), m_rmfRTV.GetResourceDesc());
		m_emissionTexture.PointToResource(m_emissionRTV.GetResource(), m_emissionRTV.GetResourceDesc());
		m_modelIDTexture.PointToResource(m_modelIDRTV.GetResource(), m_modelIDRTV.GetResourceDesc());

		m_depthTexture.CreateCopy(m_depthStencil.GetResourceDesc());
		m_copyNormalsTexture.CreateCopy(m_normalsTexture.GetResourceDesc());

		
		m_screenTexture.PointToResource(m_hdrRenderTarget.GetResource(), m_hdrRenderTarget.GetResourceDesc());

		ParticleSystem::instance().CreateCopyDepthTexture(m_depthStencil.GetResourceDesc());
	}

	void Renderer::RenderShadows()
	{
		engine::s_deviceContext->OMSetDepthStencilState(m_defDepthStencilState.ptr(), 1);
		engine::ShadowManager::instance().BindPointLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthToCubemaps();
		engine::VegetationSystem::instance().renderSceneDepthToCubemaps();

		engine::ShadowManager::instance().BindDirectionalLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthForDirectionalLights();
		engine::VegetationSystem::instance().renderSceneDepthForDirectionalLights();
	}

	void Renderer::RenderToGBuffer()
	{
		m_depthStencil.Clear();

		m_normalsRTV.Clear();
		m_albedoRTV.Clear();
		m_rmfRTV.Clear();
		m_emissionRTV.Clear();
		m_modelIDRTV.Clear();
		engine::s_deviceContext->OMSetRenderTargets(5, m_GBuffer, m_depthStencil.GetDepthStencilView().ptr());
		m_sky.BindEnvironment();

		engine::s_deviceContext->OMSetDepthStencilState(m_defDepthStencilWriteState.ptr(), 1);
		engine::MeshSystem::instance().render();

		engine::s_deviceContext->OMSetDepthStencilState(m_defDepthStencilWriteState.ptr(), 2);
		engine::VegetationSystem::instance().render();

		engine::s_deviceContext->OMSetDepthStencilState(m_disableDSS.ptr(), 1);
		engine::s_deviceContext->OMSetRenderTargets(4, m_GBuffer, m_depthStencil.GetDepthStencilView().ptr());
		m_copyNormalsTexture.Copy(m_normalsTexture.GetResource());
		m_copyNormalsTexture.Bind(1);
		m_modelIDTexture.Bind(2);
		m_depthTexture.Copy(m_depthStencil.GetDepthStencilResource());
		m_depthTexture.Bind(3);
		engine::DecalSystem::instance().render();

		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void Renderer::DeferredShading()
	{
		m_hdrRenderTarget.Clear();
		engine::s_deviceContext->OMSetRenderTargets(1, m_hdrRenderTarget.GetRTVPtrToPrt(), m_depthStencil.GetDepthStencilView().ptr());

		m_normalsTexture.Bind(0);
		m_albedoTexture.Bind(1);
		m_rmfTexture.Bind(2);
		m_emissionTexture.Bind(3);

		m_depthTexture.Copy(m_depthStencil.GetDepthStencilResource());
		m_depthTexture.Bind(4);
		m_modelIDTexture.Bind(5);

		engine::s_deviceContext->OMSetBlendState(m_additiveBlendState.ptr(), blendFactor, sampleMask);
		
		//STECIL == 0x01;
		{
			engine::s_deviceContext->OMSetDepthStencilState(m_disDepthStencilReadState.ptr(), 1);

			s_deviceContext->RSSetState(m_plRasterizerState.ptr());
			m_pl_deferredShadingShader.Bind();
			LightSystem::instance().bindPointLightClipSphere(0);
			LightSystem::instance().bindPointLightInstanceBuffer(1);
			ShadowManager::instance().m_pointLightDSResolutionBuffer.BindPS(8);
			s_deviceContext->PSSetShaderResources(8, 1, &ShadowManager::instance().m_srvPointLightShadow.ptr());
			engine::s_deviceContext->DrawIndexedInstanced(LightSystem::instance().m_pointLightClipSphere->m_vertexBuffer.Size(),
				LightSystem::instance().getPointLights().size(), 0, 0, 0);
			//engine::s_deviceContext->DrawInstanced(3, LightSystem::instance().getPointLights().size(), 0, 0);
			s_deviceContext->RSSetState(nullptr);

			
			m_dl_deferredShadingShader.Bind();
			LightSystem::instance().bindDirectionalLightInstanceBuffer(0);
			s_deviceContext->PSSetShaderResources(9, 1, &ShadowManager::instance().m_srvDirectionalLightShadow.ptr());
			engine::s_deviceContext->DrawInstanced(3, LightSystem::instance().getDirectionalLights().size(), 0, 0);

			m_env_deferredShadingShader.Bind();
			engine::s_deviceContext->Draw(3, 0);
		}

		//STECIL == 0x02;
		{
			if (VegetationSystem::instance().Size() != 0)
			{
				engine::s_deviceContext->OMSetDepthStencilState(m_disDepthStencilReadState.ptr(), 2);

				s_deviceContext->RSSetState(m_plRasterizerState.ptr());
				m_pl_grass_deferredShadingShader.Bind();
				LightSystem::instance().bindPointLightClipSphere(0);
				LightSystem::instance().bindPointLightInstanceBuffer(1);
				ShadowManager::instance().m_pointLightDSResolutionBuffer.BindPS(8);
				s_deviceContext->PSSetShaderResources(8, 1, &ShadowManager::instance().m_srvPointLightShadow.ptr());
				//engine::s_deviceContext->DrawInstanced(3, LightSystem::instance().getPointLights().size(), 0, 0);
				engine::s_deviceContext->DrawIndexedInstanced(LightSystem::instance().m_pointLightClipSphere->m_vertexBuffer.Size(),
					LightSystem::instance().getPointLights().size(), 0, 0, 0);
				s_deviceContext->RSSetState(nullptr);


				

				m_dl_grass_deferredShadingShader.Bind();
				LightSystem::instance().bindDirectionalLightInstanceBuffer(0);
				s_deviceContext->PSSetShaderResources(9, 1, &ShadowManager::instance().m_srvDirectionalLightShadow.ptr());
				engine::s_deviceContext->DrawInstanced(3, LightSystem::instance().getDirectionalLights().size(), 0, 0);

				m_env_grass_deferredShadingShader.Bind();
				engine::s_deviceContext->Draw(3, 0);
			}
		}

		engine::s_deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);

		for (uint32_t i = 0; i <= 5; ++i)
			engine::s_deviceContext->PSSetShaderResources(i, 1, SRVnullptr);
		s_deviceContext->PSSetShaderResources(8, 1, SRVnullptr);
		s_deviceContext->PSSetShaderResources(9, 1, SRVnullptr);
	}

	void Renderer::ForwardShading(const Camera& camera,	const std::chrono::steady_clock::time_point& currentTime, float deltaTime)
	{
		engine::s_deviceContext->OMSetDepthStencilState(m_defDepthStencilState.ptr(), 1);
		engine::s_deviceContext->OMSetRenderTargets(1, m_hdrRenderTarget.GetRTVPtrToPrt(), m_depthStencil.GetDepthStencilView().ptr());
		MeshSystem::instance().emissiveInstances.render();
		m_sky.Draw();


		ParticleSystem::instance().CopyDepthTexture(m_depthStencil.GetDepthStencilResource());
		ParticleSystem::instance().UpdateSmokeEmitters(camera, currentTime, deltaTime);
		ParticleSystem::instance().render();
	}

	void Renderer::Resolve(RenderTargetView& windowRenderTarget, PostProcess& postProcess)
	{
		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		engine::s_deviceContext->PSSetShaderResources(1 - m_textureRegisterIndex, 1, SRVnullptr);
		m_screenTexture.Bind(m_textureRegisterIndex);

		postProcess.Resolve(windowRenderTarget);

		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, SRVnullptr);
	}
}