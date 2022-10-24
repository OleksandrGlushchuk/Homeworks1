#pragma once
#include "Vertex.h"
#include "TriangleIndices.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <string>
#include "../math/Matr.h"
#include "../math/Box.h"
#include "Mesh.h"

namespace engine
{
	class Model
	{
	public:
		Model() {}
		std::string name;
		Box box;
		std::vector<Mesh> m_meshes;
		VertexBuffer<Vertex> m_vertexBuffer; // stores vertices of all meshes of this Model
		IndexBuffer m_indexBuffer; // stores vertex indices of all meshes of this Model
	};
}