#pragma once
#include "DxRes.hpp"
#include <unordered_map>
#include <string>
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
namespace engine
{
	class ModelManager
	{
		std::unordered_map<std::string, std::shared_ptr<Model> > m_model;
		static ModelManager* s_instance;
		ModelManager() {}
	public:
		static void init();
		static void deinit();
		static ModelManager& instance();
		std::shared_ptr<Model>& LoadModel(const std::string& path);
		
		std::shared_ptr<Model>& GetModel(const std::string& path);
		void GetMatrices(aiNode* rootNode, Model& model);
	};
}