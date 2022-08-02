#include "ModelManager.h"
#include <algorithm>
namespace engine
{
	ModelManager* ModelManager::s_instance;

	void ModelManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ModelManager;
	}
	void ModelManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}
	ModelManager& ModelManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void ModelManager::GetMatrices(aiNode* rootNode, Model& model)
	{
		for (uint32_t i = 0; i < rootNode->mNumChildren; i++)
		{
			const auto& matr  = rootNode->mChildren[i]->mTransformation.Transpose();
			model.m_meshes[i].m_constantBuffer.Init(D3D11_USAGE::D3D11_USAGE_IMMUTABLE, 0, &reinterpret_cast<const Matr<4>&>(matr));
		}
	}

	std::shared_ptr<Model>& ModelManager::LoadModel(const std::string& path)
	{
		std::unordered_map<std::string, std::shared_ptr<Model> >::iterator result;
		if ((result = m_model.find(path)) != m_model.end())
		{
			return result->second;
		}

		uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(path, flags);
		DEV_ASSERT(assimpScene);

		uint32_t numMeshes = assimpScene->mNumMeshes;

		// Load vertex data

		std::shared_ptr<Model> model(new Model);
		Model& model_ref = *model;
		model_ref.name = path;
		model_ref.box = {};
		model_ref.m_meshes.resize(numMeshes);
		model_ref.m_meshRanges.resize(numMeshes);

		GetMatrices(assimpScene->mRootNode, model_ref);

		static_assert(sizeof(Vec3) == sizeof(aiVector3D));
		static_assert(sizeof(TriangleIndices) == 3 * sizeof(uint32_t));
		uint32_t allTriangleIndicesSize = 0;
		uint32_t allVerticesSize = 0;
		uint32_t v_offset = 0;
		uint32_t i_offset = 0;

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& srcMesh = assimpScene->mMeshes[i];
			auto& dstMesh = model_ref.m_meshes[i];
			auto& dstMeshRange = model_ref.m_meshRanges[i];

			dstMeshRange.box.min = reinterpret_cast<Vec3&>(srcMesh->mAABB.mMin);
			dstMeshRange.box.max = reinterpret_cast<Vec3&>(srcMesh->mAABB.mMax);
			dstMeshRange.vertexNum = srcMesh->mNumVertices;
			dstMeshRange.indexNum = srcMesh->mNumFaces * 3;

			dstMesh.name = srcMesh->mName.C_Str();
			dstMesh.vertices.resize(srcMesh->mNumVertices);
			dstMesh.triangles.resize(srcMesh->mNumFaces);

			for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
			{
				Vertex& vertex = dstMesh.vertices[v];
				vertex.position = reinterpret_cast<Vec3&>(srcMesh->mVertices[v]);
				vertex.textureCoords = reinterpret_cast<Vec2&>(srcMesh->mTextureCoords[0][v]);
				//vertex.normal = reinterpret_cast<Vec3&>(srcMesh->mNormals[v]);
				//vertex.tangent = reinterpret_cast<Vec3&>(srcMesh->mTangents[v]);
				//vertex.bitangent = reinterpret_cast<Vec3&>(srcMesh->mBitangents[v]) * -1.f; // Flip V
			}
			allVerticesSize += srcMesh->mNumVertices;

			for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
			{
				const auto& face = srcMesh->mFaces[f];
				DEV_ASSERT(face.mNumIndices == 3);
				dstMesh.triangles[f] = *reinterpret_cast<TriangleIndices*>(face.mIndices);
			}
			allTriangleIndicesSize += srcMesh->mNumFaces;
			
			dstMeshRange.vertexOffset = v_offset;
			dstMeshRange.indexOffset = i_offset;

			v_offset += srcMesh->mNumVertices;
			i_offset += srcMesh->mNumFaces * 3;
		}

		{
			std::vector<TriangleIndices> allTriangleIndices(allTriangleIndicesSize);
			std::vector<TriangleIndices>::iterator it_triangles = allTriangleIndices.begin();

			std::vector<Vertex> allVertices(allVerticesSize);
			std::vector<Vertex>::iterator it_vertices = allVertices.begin();

			for (uint32_t i = 0; i < numMeshes; i++)
			{
				it_triangles = std::copy(model_ref.m_meshes[i].triangles.begin(), model_ref.m_meshes[i].triangles.end(), it_triangles);
				it_vertices = std::copy(model_ref.m_meshes[i].vertices.begin(), model_ref.m_meshes[i].vertices.end(), it_vertices);
			}
			model_ref.m_indexBuffer.Init(allTriangleIndices);
			model_ref.m_vertexBuffer.Init(allVertices);
		}
		return m_model[path] = model;
	}
	std::shared_ptr<Model>& ModelManager::GetModel(const std::string& path)
	{
		std::unordered_map<std::string, std::shared_ptr<Model> >::iterator result;
		ALWAYS_ASSERT((result = m_model.find(path)) != m_model.end() && "Bad path");
		return result->second;
	}
}
