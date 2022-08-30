#pragma once
#include <vector>
#include <string>
#include "Vertex.h"
#include "TriangleIndices.h"
#include "../math/Matr.h"
#include "../math/Box.h"
#include "TriangleOctree.hpp"

namespace engine
{
	struct Mesh
	{
		Mesh() {}
		void InitTriangleOctree() { if(!triangleOctree.inited()) triangleOctree.initialize(*this); }
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<TriangleIndices> triangles;
		Matr<4> meshToModelMatrix;
		Box box;
		TriangleOctree triangleOctree;

		uint32_t vertexOffset; // offset in vertices
		uint32_t indexOffset; // offset in indices
		uint32_t vertexNum; // num of vertices
		uint32_t indexNum; // num of indices
	};
}