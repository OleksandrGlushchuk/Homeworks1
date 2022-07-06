#pragma once
#include <initializer_list>

struct Vertex2D
{
	float x, y;
	float r, g, b;
	inline void SetVertex(const float _x, const float _y, const float _r, const float _g, const float _b)
	{
		x = _x;
		y = _y;
		r = _r;
		g = _g;
		b = _b;
	}
};


template<int SIZE, typename VertexType = Vertex2D>
class VertexMesh
{
	VertexType vertices[SIZE];
public:
	inline VertexType& operator[](int i)
	{
		return vertices[i];
	}

	inline const VertexType& operator[](int i) const
	{
		return vertices[i];
	}

	inline VertexType* GetPointerToVertices()
	{
		return vertices;
	}

	inline int MeshSize() const
	{
		return sizeof(vertices);
	}

	inline int VertexSize() const
	{
		return sizeof(VertexType);
	}
};