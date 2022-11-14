#include "GrassField.h"
#include "singletones/BlendStateManager.h"
#include "singletones/RasteriserStateManager.h"
#include "singletones/ShadowManager.h"
#include "singletones/LightSystem.h"
#include "singletones/DepthStencilManager.h"
#include "singletones/SamplerManager.h"
#include "singletones/ModelManager.h"
#include <vector>
#include <random>
#include <iterator>

static ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };

namespace engine
{
	void GrassField::Init()
	{
		ModelManager::instance().InitGrassModel(8);
		m_model = ModelManager::instance().GetGrassModel();

		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"GRASS_POS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		m_shader.Init(L"engine/shaders/grass.hlsl", desc, 3, ShaderEnabling(true, true));
		ShadowManager::instance().m_pointLightGrassShadowShader.Init(L"engine/shaders/grass_shadow_point_light.hlsl", desc, 3, ShaderEnabling(true, true));
		ShadowManager::instance().m_directionalLightGrassShadowShader.Init(L"engine/shaders/grass_shadow_directional_light.hlsl", desc, 3, ShaderEnabling(true, true));


		MaterialConstantBuffer mcb(false, true, true, false);
		m_materialConstantBuffer.Init(D3D11_USAGE_DEFAULT, 0, &mcb);
		
		m_grassColor.Load(L"engine/assets/Grass/Albedo.dds");
		m_grassRoughness.Load(L"engine/assets/Grass/Roughness.dds");
		m_grassNormal.Load(L"engine/assets/Grass/Normal.dds");
		m_grassAO.Load(L"engine/assets/Grass/AO.dds");
		m_grassOpacity.Load(L"engine/assets/Grass/Opacity.dds");
		m_grassTranslucency.Load(L"engine/assets/Grass/Translucency.dds");
		m_rasterizerState = RasteriserStateManager::instance().GetRasteriserState("front_and_back");
		m_blendState = BlendStateManager::instance().GetBlendState("alphaToCoverage");
	}

	void GrassField::AddGrassArea(float width, float height, float r, const Vec3& world_pos)
	{
		typedef std::uniform_real<float>::param_type frange;
		typedef std::uniform_int<int>::param_type irange;
		std::default_random_engine r_engine;
		std::uniform_real_distribution<float> frandom;
		std::uniform_int_distribution<int> irandom;


		float w = r / sqrtf(2.f);
		int cols = width / w;
		int& elems_in_row = cols;
		int rows = height / w;
		int num_elems = cols * rows;

		ALWAYS_ASSERT(num_elems > 0 && "Bad grassField size");

		std::vector<int> active_index;
		active_index.reserve(num_elems);

		std::vector<GrassInstance> elem(num_elems, GrassInstance({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() , std::numeric_limits<float>::infinity() }));

		float x = frandom(r_engine, frange(0, width));
		float z = frandom(r_engine, frange(0, height));

		int k = 30;

		int xi = x / w, zi = z / w;

		elem[zi * elems_in_row + xi].position = Vec3(x, 0, z);
		active_index.emplace_back(zi * elems_in_row + xi);

		int sample_index;
		while (!active_index.empty())
		{
			sample_index = active_index[irandom(r_engine, irange(0, active_index.size()-1))];
			Vec3 active_pos = elem[sample_index].position;
			bool good_sample = false;
			for (int n = 0; n < k; ++n)
			{
				float angle = M_PI * 2.f / n;
				float distance = frandom(r_engine, frange(r, 2 * r));
				float c = cosf(angle);
				float s = sinf(angle);
				x = distance * c;
				z = distance * s;

				Vec3 sample(x, 0, z);
				sample += active_pos;

				int sample_col = sample.x() / w;
				int sample_row = sample.z() / w;

				bool good_position = true;
				if (sample_col > -1 && sample_col < cols && sample_row > -1 && sample_row < rows && elem[sample_row * elems_in_row + sample_col].position.e[0] == std::numeric_limits<float>::infinity())
				{
					for (int i = -1; i <= 1; ++i)
					{
						for (int j = -1; j <= 1; ++j)
						{
							int neighbor_col = j + sample_col;
							int neightbor_row = i + sample_row;
							

							if (neighbor_col > -1 && neighbor_col < cols && neightbor_row > -1 && neightbor_row < rows)
							{
								int neighbor_index = neightbor_row * elems_in_row + neighbor_col;
								Vec3 neighbor_pos = elem[neighbor_index].position;
								if (neighbor_pos.e[0] != std::numeric_limits<float>::infinity())
								{
									float distance_sample_neighbor = (neighbor_pos - sample).length();
									if (distance_sample_neighbor < r)
									{
										good_position = false;
										goto l1;
									}
								}
							}
						}
					}
				l1:
					if (good_position)
					{
						elem[sample_row * elems_in_row + sample_col].position = sample;
						active_index.emplace_back(sample_row * elems_in_row + sample_col);
						good_sample = true;
						//break;
					}
				}
			}
			if (!good_sample)
			{
				active_index.erase( std::find(active_index.begin(), active_index.end(), sample_index) );
			}
		}
		auto it = std::remove_if(elem.begin(), elem.end(), [](GrassInstance& i) { return i.position.e[0] == std::numeric_limits<float>::infinity(); });
		elem.erase(it, elem.end());

		std::transform(elem.begin(), elem.end(), elem.begin(), [&world_pos](GrassInstance& i) {return GrassInstance(i.position + world_pos); });
		
		uint32_t prev_size = m_instances.size();
		m_instances.resize(prev_size + elem.size());

		auto insert_it = m_instances.begin();
		std::advance(insert_it, prev_size);
		
		std::copy(elem.begin(), elem.end(), insert_it);
		
		m_instanceBuffer.Init(m_instances);
	}

	FLOAT blendFactor[4] = { 0,0,0,0 };
	UINT sampleMask = 0xffffffff;

	void GrassField::render()
	{
		if (m_instanceBuffer.Size() == 0)
			return;

		engine::s_deviceContext->RSSetState(m_rasterizerState.ptr());

		m_shader.Bind();
		m_model->m_vertexBuffer.Bind(0);
		m_model->m_indexBuffer.Bind();
		m_instanceBuffer.Bind(1);
		m_materialConstantBuffer.BindPS(1);

		m_grassColor.Bind(0);
		m_grassNormal.Bind(1);
		m_grassRoughness.Bind(2);
		m_grassMetalness.Bind(3);
		m_grassAO.Bind(4);
		m_grassOpacity.Bind(5);
		m_grassTranslucency.Bind(6);

		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		engine::s_deviceContext->DrawIndexedInstanced(m_model->m_meshes[0].indexNum, m_instanceBuffer.Size(), 0, 0, 0);

		engine::s_deviceContext->RSSetState(nullptr);
	}

	void GrassField::renderSceneDepthToCubemaps()
	{
		uint32_t pointLightNum = LightSystem::instance().getPointLights().size();
		if (pointLightNum == 0)
			return;
		engine::s_deviceContext->OMSetBlendState(m_blendState.ptr(), blendFactor, sampleMask);
		engine::s_deviceContext->RSSetState(m_rasterizerState.ptr());

		ShadowManager::instance().m_pointLightGrassShadowShader.Bind();
		ShadowManager::instance().m_pointLightShadowBuffer.BindGS(2);

		m_instanceBuffer.Bind(1);
		m_model->m_vertexBuffer.Bind(0);
		m_model->m_indexBuffer.Bind();
		m_grassOpacity.Bind(0);

		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (uint32_t i = 0; i < pointLightNum; ++i)
		{
			ShadowManager::instance().m_pointLightShadowBuffer.Update(ShadowManager::PointLightShadowBuffer(i));
			engine::s_deviceContext->DrawIndexedInstanced(m_model->m_meshes[0].indexNum, m_instanceBuffer.Size(), 0, 0, 0);
		}

		engine::s_deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);
		engine::s_deviceContext->RSSetState(nullptr);
	}

	void GrassField::renderSceneDepthForDirectionalLights()
	{
		uint32_t directionalLightNum = LightSystem::instance().getDirectionalLights().size();

		if (directionalLightNum == 0)
			return;

		engine::s_deviceContext->OMSetBlendState(m_blendState.ptr(), blendFactor, sampleMask);
		engine::s_deviceContext->RSSetState(m_rasterizerState.ptr());

		ShadowManager::instance().m_directionalLightGrassShadowShader.Bind();
		ShadowManager::instance().m_directionalLightShadowBuffer.BindGS(2);

		m_instanceBuffer.Bind(1);
		m_model->m_vertexBuffer.Bind(0);
		m_model->m_indexBuffer.Bind();
		m_grassOpacity.Bind(0);

		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (uint32_t i = 0; i < directionalLightNum; ++i)
		{
			ShadowManager::instance().m_directionalLightShadowBuffer.Update(ShadowManager::DirectionalLightShadowBuffer(i));
			engine::s_deviceContext->DrawIndexedInstanced(m_model->m_meshes[0].indexNum, m_instanceBuffer.Size(), 0, 0, 0);
		}

		engine::s_deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);
		engine::s_deviceContext->RSSetState(nullptr);
	}
}
