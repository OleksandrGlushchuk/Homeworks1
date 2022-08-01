#pragma once
#include "Vertex.h"
#include "TriangleIndices.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <string>
#include "../math/Transform.h"
struct Box
{
	Vec3 min;
	Vec3 max;
};

class Model
{
public:
	Model(){}
	struct MeshRange
	{
		MeshRange(){}
		uint32_t vertexOffset; // offset in vertices
		uint32_t indexOffset; // offset in indices
		uint32_t vertexNum; // num of vertices
		uint32_t indexNum; // num of indices
		Box box;
	};
	struct Mesh
	{
		Mesh(){}
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<TriangleIndices> triangles;
		Matr<4> meshToModelMatrix;
	};
	std::string name;
	Box box;
	std::vector<MeshRange> m_meshRanges;
	std::vector<Mesh> m_meshes;
	VertexBuffer<Vertex> m_vertexBuffer; // stores vertices of all meshes of this Model
	IndexBuffer m_indexBuffer; // stores vertex indices of all meshes of this Model
};