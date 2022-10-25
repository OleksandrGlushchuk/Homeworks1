#include "ModelManager.h"
#include <algorithm>
namespace engine
{
	ModelManager* ModelManager::s_instance;

	void ModelManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ModelManager;
		s_instance->InitUnitQuadModel();
		s_instance->InitUnitCubeModel();
		s_instance->initUnitSphereModel();
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

	void ModelManager::InitGrassModel(uint32_t numSegments)
	{
		if (m_GrassModelIsInited)
			return;
		std::shared_ptr<Model> grass_model(new Model);
		auto& model = *grass_model;
		model.name = "Grass";
		model.box = Box::unit();

		model.m_meshes.resize(1);

		auto& mesh = model.m_meshes[0];
		mesh.box = Box::unit();
		mesh.meshToModelMatrix = Matr<4>::identity();
		mesh.name = "Grass";
		mesh.vertices.resize(numSegments * 2 + 2);
		mesh.triangles.resize(numSegments * 2);
		mesh.indexNum = 6 * numSegments;
		mesh.indexOffset = 0;
		mesh.vertexOffset = 0;
		mesh.vertexNum = mesh.vertices.size();
		Vec3 tangent, bitangent, normal;
		tangent = { 1, 0, 0 };
		bitangent = { 0, -1, 0 };
		normal = { 0,0,-1 };

		uint32_t numRibs = numSegments + 1;
		float yStep = 1.f / numSegments;
		float vStep = yStep;

		for (uint32_t i = 0, j = 0; i < mesh.vertexNum; i+=2, j+= 1)
		{
			mesh.vertices[i].position = Vec3(-0.5f, 0.f + yStep*j, 0.f);
			mesh.vertices[i].textureCoords = Vec2(0, std::max(0.f,1.f - vStep*j));
			mesh.vertices[i].normal = normal;
			mesh.vertices[i].tangent = tangent;
			mesh.vertices[i].bitangent = bitangent;

			mesh.vertices[i+1].position = Vec3(0.5f, 0.f + yStep*j, 0.f);
			mesh.vertices[i+1].textureCoords = Vec2(1, std::max(0.f, 1.f - vStep * j));
			mesh.vertices[i+1].normal = normal;
			mesh.vertices[i+1].tangent = tangent;
			mesh.vertices[i+1].bitangent = bitangent;
		}

		for (uint32_t i = 2; i < mesh.vertexNum; i += 2)
		{
			mesh.triangles[i - 2] = TriangleIndices(i-2, i, i+1);
			mesh.triangles[i - 1] = TriangleIndices(i+1, i-1, i-2);
		}
		model.m_indexBuffer.Init(mesh.triangles);
		model.m_vertexBuffer.Init(mesh.vertices);


		m_defaultModel[grass_model->name] = grass_model;
		m_GrassModelIsInited = true;
	}

	void ModelManager::InitUnitQuadModel()
	{
		if (m_UnitQuadModelIsInited)
			return;
		std::shared_ptr<Model> quad_model(new Model);
		auto& model = *quad_model;
		model.name = "Quad";
		model.box = Box::unit();

		model.m_meshes.resize(1);

		auto& mesh = model.m_meshes[0];
		mesh.box = Box::unit();
		mesh.meshToModelMatrix = Matr<4>::identity();
		mesh.name = "Quad";
		mesh.vertices.resize(4);
		mesh.triangles.resize(2);
		mesh.indexNum = 6;
		mesh.indexOffset = 0;
		mesh.vertexOffset = 0;
		mesh.vertexNum = 4;
		Vec3 tangent, bitangent, normal;
		tangent = { 1, 0, 0 };
		bitangent = { 0, -1, 0 };
		normal = { 0,0,-1 };

		mesh.vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		mesh.vertices[0].textureCoords = Vec2(0, 1);
		mesh.vertices[0].normal = normal;
		mesh.vertices[0].tangent = tangent;
		mesh.vertices[0].bitangent = bitangent;

		mesh.vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
		mesh.vertices[1].textureCoords = Vec2(0, 0);
		mesh.vertices[1].normal = normal;
		mesh.vertices[1].tangent = tangent;
		mesh.vertices[1].bitangent = bitangent;

		mesh.vertices[2].position = Vec3(0.5f, 0.5f, 0.f);
		mesh.vertices[2].textureCoords = Vec2(1, 0);
		mesh.vertices[2].normal = normal;
		mesh.vertices[2].tangent = tangent;
		mesh.vertices[2].bitangent = bitangent;

		mesh.vertices[3].position = Vec3(0.5f, -0.5f, 0.f);
		mesh.vertices[3].textureCoords = Vec2(1, 1);
		mesh.vertices[3].normal = normal;
		mesh.vertices[3].tangent = tangent;
		mesh.vertices[3].bitangent = bitangent;


		mesh.triangles[0] = TriangleIndices(0, 1, 2);
		mesh.triangles[1] = TriangleIndices(2, 3, 0);

		model.m_indexBuffer.Init(mesh.triangles);
		model.m_vertexBuffer.Init(mesh.vertices);
		mesh.InitTriangleOctree();

		m_defaultModel[quad_model->name] = quad_model;
		m_UnitQuadModelIsInited = true;
	}

	void ModelManager::InitUnitCubeModel()
	{
		if (m_UnitCubeModelIsInited)
			return;
		std::shared_ptr<Model> cube_model(new Model);
		auto& model = *cube_model;
		model.name = "Cube";
		model.box = Box::unit();

		model.m_meshes.resize(1);

		auto& mesh = model.m_meshes[0];
		mesh.box = Box::unit();
		mesh.meshToModelMatrix = Matr<4>::identity();
		mesh.name = "Cube";
		mesh.vertices.resize(24);
		mesh.triangles.resize(12);
		mesh.indexNum = 36;
		mesh.indexOffset = 0;
		mesh.vertexOffset = 0;
		mesh.vertexNum = 24;

		Vec3 tangent, bitangent, normal;
		
		//FACE
		{
			tangent = { 1, 0, 0 };
			bitangent = { 0, -1, 0 };
			normal = { 0,0,-1 };
			mesh.vertices[0].position = Vec3(-0.5f, -0.5f, -0.5f);
			mesh.vertices[0].textureCoords = Vec2(0, 1);
			mesh.vertices[0].normal = normal;
			mesh.vertices[0].tangent = tangent;
			mesh.vertices[0].bitangent = bitangent;

			mesh.vertices[1].position = Vec3(-0.5f, 0.5f, -0.5f);
			mesh.vertices[1].textureCoords = Vec2(0, 0);
			mesh.vertices[1].normal = normal;
			mesh.vertices[1].tangent = tangent;
			mesh.vertices[1].bitangent = bitangent;

			mesh.vertices[2].position = Vec3(0.5f, 0.5f, -0.5f);
			mesh.vertices[2].textureCoords = Vec2(1, 0);
			mesh.vertices[2].normal = normal;
			mesh.vertices[2].tangent = tangent;
			mesh.vertices[2].bitangent = bitangent;

			mesh.vertices[3].position = Vec3(0.5f, -0.5f, -0.5f);
			mesh.vertices[3].textureCoords = Vec2(1, 1);
			mesh.vertices[3].normal = normal;
			mesh.vertices[3].tangent = tangent;
			mesh.vertices[3].bitangent = bitangent;
		}

		//BACK
		{
			tangent = { -1, 0, 0 };
			bitangent = { 0, -1, 0 };
			normal = { 0,0,1 };
			mesh.vertices[4].position = Vec3(0.5f, -0.5f, 0.5f);
			mesh.vertices[4].textureCoords = Vec2(0, 1);
			mesh.vertices[4].normal = normal;
			mesh.vertices[4].tangent = tangent;
			mesh.vertices[4].bitangent = bitangent;

			mesh.vertices[5].position = Vec3(0.5f, 0.5f, 0.5f);
			mesh.vertices[5].textureCoords = Vec2(0, 0);
			mesh.vertices[5].normal = normal;
			mesh.vertices[5].tangent = tangent;
			mesh.vertices[5].bitangent = bitangent;

			mesh.vertices[6].position = Vec3(-0.5f, 0.5f, 0.5f);
			mesh.vertices[6].textureCoords = Vec2(1, 0);
			mesh.vertices[6].normal = normal;
			mesh.vertices[6].tangent = tangent;
			mesh.vertices[6].bitangent = bitangent;

			mesh.vertices[7].position = Vec3(-0.5f, -0.5f, 0.5f);
			mesh.vertices[7].textureCoords = Vec2(1, 1);
			mesh.vertices[7].normal = normal;
			mesh.vertices[7].tangent = tangent;
			mesh.vertices[7].bitangent = bitangent;
		}

		//UP
		{
			tangent = { 1, 0, 0 };
			bitangent = { 0, 0, -1 };
			normal = { 0,1,0 };
			mesh.vertices[8].position = mesh.vertices[1].position;
			mesh.vertices[8].textureCoords = Vec2(0, 1);
			mesh.vertices[8].normal = normal;
			mesh.vertices[8].tangent = tangent;
			mesh.vertices[8].bitangent = bitangent;

			mesh.vertices[9].position = mesh.vertices[6].position;
			mesh.vertices[9].textureCoords = Vec2(0, 0);
			mesh.vertices[9].normal = normal;
			mesh.vertices[9].tangent = tangent;
			mesh.vertices[9].bitangent = bitangent;

			mesh.vertices[10].position = mesh.vertices[5].position;
			mesh.vertices[10].textureCoords = Vec2(1, 0);
			mesh.vertices[10].normal = normal;
			mesh.vertices[10].tangent = tangent;
			mesh.vertices[10].bitangent = bitangent;

			mesh.vertices[11].position = mesh.vertices[2].position;
			mesh.vertices[11].textureCoords = Vec2(1, 1);
			mesh.vertices[11].normal = normal;
			mesh.vertices[11].tangent = tangent;
			mesh.vertices[11].bitangent = bitangent;
		}

		//DOWN
		{
			tangent = { 1, 0, 0 };
			bitangent = { 0, 0, 1 };
			normal = { 0,-1,0 };
			mesh.vertices[12].position = mesh.vertices[7].position;
			mesh.vertices[12].textureCoords = Vec2(0, 1);
			mesh.vertices[12].normal = normal;
			mesh.vertices[12].tangent = tangent;
			mesh.vertices[12].bitangent = bitangent;

			mesh.vertices[13].position = mesh.vertices[0].position;
			mesh.vertices[13].textureCoords = Vec2(0, 0);
			mesh.vertices[13].normal = normal;
			mesh.vertices[13].tangent = tangent;
			mesh.vertices[13].bitangent = bitangent;

			mesh.vertices[14].position = mesh.vertices[3].position;
			mesh.vertices[14].textureCoords = Vec2(1, 0);
			mesh.vertices[14].normal = normal;
			mesh.vertices[14].tangent = tangent;
			mesh.vertices[14].bitangent = bitangent;

			mesh.vertices[15].position = mesh.vertices[4].position;
			mesh.vertices[15].textureCoords = Vec2(1, 1);
			mesh.vertices[15].normal = normal;
			mesh.vertices[15].tangent = tangent;
			mesh.vertices[15].bitangent = bitangent;
		}

		//LEFT
		{
			tangent = { 0, 0, -1 };
			bitangent = { 0, -1, 0 };
			normal = { -1,0,0 };
			mesh.vertices[16].position = mesh.vertices[7].position;
			mesh.vertices[16].textureCoords = Vec2(0, 1);
			mesh.vertices[16].normal = normal;
			mesh.vertices[16].tangent = tangent;
			mesh.vertices[16].bitangent = bitangent;

			mesh.vertices[17].position = mesh.vertices[6].position;
			mesh.vertices[17].textureCoords = Vec2(0, 0);
			mesh.vertices[17].normal = normal;
			mesh.vertices[17].tangent = tangent;
			mesh.vertices[17].bitangent = bitangent;

			mesh.vertices[18].position = mesh.vertices[1].position;
			mesh.vertices[18].textureCoords = Vec2(1, 0);
			mesh.vertices[18].normal = normal;
			mesh.vertices[18].tangent = tangent;
			mesh.vertices[18].bitangent = bitangent;

			mesh.vertices[19].position = mesh.vertices[0].position;
			mesh.vertices[19].textureCoords = Vec2(1, 1);
			mesh.vertices[19].normal = normal;
			mesh.vertices[19].tangent = tangent;
			mesh.vertices[19].bitangent = bitangent;
		}

		//RIGHT
		{
			tangent = { 0, 0, 1 };
			bitangent = { 0, -1, 0 };
			normal = { 1,0,0 };
			mesh.vertices[20].position = mesh.vertices[3].position;
			mesh.vertices[20].textureCoords = Vec2(0, 1);
			mesh.vertices[20].normal = normal;
			mesh.vertices[20].tangent = tangent;
			mesh.vertices[20].bitangent = bitangent;

			mesh.vertices[21].position = mesh.vertices[2].position;
			mesh.vertices[21].textureCoords = Vec2(0, 0);
			mesh.vertices[21].normal = normal;
			mesh.vertices[21].tangent = tangent;
			mesh.vertices[21].bitangent = bitangent;

			mesh.vertices[22].position = mesh.vertices[5].position;
			mesh.vertices[22].textureCoords = Vec2(1, 0);
			mesh.vertices[22].normal = normal;
			mesh.vertices[22].tangent = tangent;
			mesh.vertices[22].bitangent = bitangent;

			mesh.vertices[23].position = mesh.vertices[4].position;
			mesh.vertices[23].textureCoords = Vec2(1, 1);
			mesh.vertices[23].normal = normal;
			mesh.vertices[23].tangent = tangent;
			mesh.vertices[23].bitangent = bitangent;
		}


		mesh.triangles[0] = TriangleIndices(0, 1, 2);
		mesh.triangles[1] = TriangleIndices(2, 3, 0);

		mesh.triangles[2] = TriangleIndices(4, 5, 6);
		mesh.triangles[3] = TriangleIndices(6, 7, 4);

		mesh.triangles[4] = TriangleIndices(8, 9, 10);
		mesh.triangles[5] = TriangleIndices(10, 11, 8);

		mesh.triangles[6] = TriangleIndices(12, 13, 14);
		mesh.triangles[7] = TriangleIndices(14, 15, 12);

		mesh.triangles[8] = TriangleIndices(16, 17, 18);
		mesh.triangles[9] = TriangleIndices(18, 19, 16);

		mesh.triangles[10] = TriangleIndices(20, 21, 22);
		mesh.triangles[11] = TriangleIndices(22, 23, 20);

		

		model.m_indexBuffer.Init(mesh.triangles);
		model.m_vertexBuffer.Init(mesh.vertices);
		mesh.InitTriangleOctree();

		m_defaultModel[cube_model->name] = cube_model;
		m_UnitCubeModelIsInited = true;
	}

	void ModelManager::initUnitSphereModel()
	{
		if (m_UnitSphereModelIsInited)
			return;
		const uint32_t SIDES = 6;
		const uint32_t GRID_SIZE = 12;
		const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

		std::shared_ptr<Model> sphere_model(new Model);
		Model& model = *sphere_model;
		model.name = "Sphere";
		model.box = Box::unit();

		auto& mesh = model.m_meshes.emplace_back();
		mesh.name = "Sphere";
		mesh.box = Box::unit();
		mesh.meshToModelMatrix = Matr<4>::identity();
		mesh.vertices.resize(VERT_PER_SIZE * SIDES);
		mesh.triangles.resize(VERT_PER_SIZE * SIDES / 3);
		mesh.indexOffset = 0;
		mesh.vertexOffset = 0;
		mesh.vertexNum = VERT_PER_SIZE * SIDES;
		mesh.indexNum = mesh.triangles.size() * 3;

		auto* vertex = mesh.vertices.data();
		auto* triangle = mesh.triangles.data();
		
		int sideMasks[6][3] =
		{
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 0, 2, 1 },
			{ 0, 2, 1 }
		};

		float sideSigns[6][3] =
		{
			{ +1, +1, +1 },
			{ -1, +1, +1 },
			{ -1, +1, -1 },
			{ +1, +1, -1 },
			{ +1, -1, -1 },
			{ +1, +1, +1 }
		};

		for (int side = 0; side < SIDES; ++side)
		{
			for (int row = 0; row < GRID_SIZE; ++row)
			{
				for (int col = 0; col < GRID_SIZE; ++col)
				{
					float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
					float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

					Vec3 quad[4] =
					{
						{ left, bottom, 1.f },
						{ right, bottom, 1.f },
						{ left, top, 1.f },
						{ right, top, 1.f }
					};

					vertex[0] = vertex[1] = vertex[2] = Vertex{ {0,0,0},{0,0},{0,0,0},{0,0,0},{0,0,0}};
					
					auto setPos = [sideMasks, sideSigns](int side, Vertex& dst, const Vec3& pos)
					{
						dst.position[sideMasks[side][0]] = pos.x() * sideSigns[side][0];
						dst.position[sideMasks[side][1]] = pos.y() * sideSigns[side][1];
						dst.position[sideMasks[side][2]] = pos.z() * sideSigns[side][2];
						dst.position = dst.position.normalized();
					};

					setPos(side, vertex[0], quad[0]);
					setPos(side, vertex[1], quad[1]);
					setPos(side, vertex[2], quad[2]);

					{
						vertex[0].normal = vertex[0].position;
						vertex[1].normal = vertex[1].position;
						vertex[2].normal = vertex[2].position;
					}

					triangle[0] = TriangleIndices(vertex - mesh.vertices.data() + 2, vertex - mesh.vertices.data() + 1, vertex - mesh.vertices.data());
					triangle += 1;
					vertex += 3;

					vertex[0] = vertex[1] = vertex[2] = Vertex{ {0,0,0},{0,0},{0,0,0},{0,0,0},{0,0,0} };

					setPos(side, vertex[0], quad[1]);
					setPos(side, vertex[1], quad[3]);
					setPos(side, vertex[2], quad[2]);

					{
						vertex[0].normal = vertex[0].position;
						vertex[1].normal = vertex[1].position;
						vertex[2].normal = vertex[2].position;
					}

					triangle[0] = TriangleIndices(vertex - mesh.vertices.data()+2, vertex - mesh.vertices.data() + 1, vertex - mesh.vertices.data());
					triangle += 1;
					vertex += 3;
				}
			}
		}

		model.m_indexBuffer.Init(model.m_meshes[0].triangles);
		model.m_vertexBuffer.Init(model.m_meshes[0].vertices);
		mesh.InitTriangleOctree();
		m_defaultModel[sphere_model->name] = sphere_model;
		m_UnitSphereModelIsInited = true;
	}

	void ModelManager::GetMatrices(aiNode* rootNode, Model& model)
	{
		for (uint32_t i = 0; i < rootNode->mNumChildren; i++)
		{
			const auto& matr  = rootNode->mChildren[i]->mTransformation.Transpose();
			model.m_meshes[i].meshToModelMatrix = reinterpret_cast<const Matr<4>&>(matr);
		}
	}

	const std::shared_ptr<Model>& ModelManager::LoadModel(const std::string& path)
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

			dstMesh.name = srcMesh->mName.C_Str();
			dstMesh.vertices.resize(srcMesh->mNumVertices);
			dstMesh.triangles.resize(srcMesh->mNumFaces);
			dstMesh.box.min = reinterpret_cast<Vec3&>(srcMesh->mAABB.mMin);
			dstMesh.box.max = reinterpret_cast<Vec3&>(srcMesh->mAABB.mMax);
			dstMesh.vertexNum = srcMesh->mNumVertices;
			dstMesh.indexNum = srcMesh->mNumFaces * 3;

			for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
			{
				Vertex& vertex = dstMesh.vertices[v];
				vertex.position = reinterpret_cast<Vec3&>(srcMesh->mVertices[v]);
				vertex.textureCoords = reinterpret_cast<Vec2&>(srcMesh->mTextureCoords[0][v]);
				vertex.normal = reinterpret_cast<Vec3&>(srcMesh->mNormals[v]);
				vertex.tangent = reinterpret_cast<Vec3&>(srcMesh->mTangents[v]);
				vertex.bitangent = reinterpret_cast<Vec3&>(srcMesh->mBitangents[v]);
			}
			allVerticesSize += srcMesh->mNumVertices;

			for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
			{
				const auto& face = srcMesh->mFaces[f];
				DEV_ASSERT(face.mNumIndices == 3);
				dstMesh.triangles[f] = *reinterpret_cast<TriangleIndices*>(face.mIndices);
			}
			allTriangleIndicesSize += srcMesh->mNumFaces;
			
			dstMesh.vertexOffset = v_offset;
			dstMesh.indexOffset = i_offset;

			v_offset += srcMesh->mNumVertices;
			i_offset += srcMesh->mNumFaces * 3;
		}

		{
			std::vector<TriangleIndices> allTriangleIndices(allTriangleIndicesSize);
			std::vector<TriangleIndices>::iterator it_triangles = allTriangleIndices.begin();

			std::vector<Vertex> allVertices(allVerticesSize);
			std::vector<Vertex>::iterator it_vertices = allVertices.begin();

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				it_triangles = std::copy(model_ref.m_meshes[i].triangles.begin(), model_ref.m_meshes[i].triangles.end(), it_triangles);
				it_vertices = std::copy(model_ref.m_meshes[i].vertices.begin(), model_ref.m_meshes[i].vertices.end(), it_vertices);
			}
			model_ref.m_indexBuffer.Init(allTriangleIndices);
			model_ref.m_vertexBuffer.Init(allVertices);
		}
		for (auto& mesh : model->m_meshes)
		{
			mesh.InitTriangleOctree();
		}
		return m_model[path] = model;
	}

	const std::shared_ptr<Model>& ModelManager::GetModel(const std::string& path)
	{
		std::unordered_map<std::string, std::shared_ptr<Model> >::iterator result;
		ALWAYS_ASSERT((result = m_model.find(path)) != m_model.end() && "Bad path");
		return result->second;
	}
}
