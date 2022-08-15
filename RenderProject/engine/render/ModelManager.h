#pragma once
#include "DxRes.hpp"
#include <unordered_map>
#include <string>
#include "Model.h"
#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"
#include <memory>
namespace engine
{
	class ModelManager
	{
		std::unordered_map<std::string, std::shared_ptr<Model> > m_model;
		std::unordered_map<std::string, std::shared_ptr<Model> > m_defaultModel;
		static ModelManager* s_instance;
		ModelManager() {}
		ModelManager(const ModelManager& other) = delete;
		ModelManager& operator=(const ModelManager& other) = delete;
		ModelManager(ModelManager&& other) noexcept = delete;
		ModelManager& operator=(ModelManager&& other) noexcept = delete;
		void InitUnitQuadModel();
		void InitUnitCubeModel();
		void initUnitSphereModel();
		void GetMatrices(aiNode* rootNode, Model& model);
	public:
		static void init();
		static void deinit();
		static ModelManager& instance();
		const std::shared_ptr<Model>& LoadModel(const std::string& path);
		const std::shared_ptr<Model>& GetModel(const std::string& path);
		const std::shared_ptr<Model>& GetUnitQuadModel() { return m_defaultModel["Quad"]; }
		const std::shared_ptr<Model>& GetUnitCubeModel() { return m_defaultModel["Cube"]; }
		const std::shared_ptr<Model>& GetUnitSphereModel() { return m_defaultModel["Sphere"]; }
	};
}