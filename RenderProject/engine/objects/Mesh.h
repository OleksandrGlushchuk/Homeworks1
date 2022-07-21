#pragma once
#include "../render/DxRes.hpp"
#include <initializer_list>

namespace MeshType
{
	struct Vertex2D
	{
		Vertex2D() {}

		Vertex2D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}

		float coord[2];
		inline void SetVertex(const float _x, const float _y)
		{
			coord[0] = _x;
			coord[1] = _y;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_UNKNOWN;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_UNKNOWN;
	};

	struct ColoredVertex2D
	{
		ColoredVertex2D() {}

		ColoredVertex2D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}

		float coord[5];
		inline void SetVertex(const float _x, const float _y, const float _r, const float _g, const float _b)
		{
			coord[0] = _x;
			coord[1] = _y;
			coord[2] = _r;
			coord[3] = _g;
			coord[4] = _b;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_UNKNOWN;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
	};

	struct TexturedVertex2D
	{
		TexturedVertex2D() {}

		TexturedVertex2D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}
		float coord[4];
		inline void SetVertex(const float _x, const float _y, const float _tx, const float _ty)
		{
			coord[0] = _x;
			coord[1] = _y;
			coord[2] = _tx;
			coord[3] = _ty;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_UNKNOWN;
	};

	struct Vertex3D
	{
		Vertex3D() {}

		Vertex3D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}

		float coord[3];
		inline void SetVertex(const float _x, const float _y, const float _z)
		{
			coord[0] = _x;
			coord[1] = _y;
			coord[2] = _z;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_UNKNOWN;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_UNKNOWN;
	};

	struct ColoredVertex3D
	{
		ColoredVertex3D() {}

		ColoredVertex3D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}

		float coord[6];
		inline void SetVertex(const float _x, const float _y, const float _z, const float _r, const float _g, const float _b)
		{
			coord[0] = _x;
			coord[1] = _y;
			coord[2] = _z;
			coord[3] = _r;
			coord[4] = _g;
			coord[5] = _b;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_UNKNOWN;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
	};

	struct TexturedVertex3D
	{
		TexturedVertex3D(){}

		TexturedVertex3D(const std::initializer_list<float>& init)
		{
			std::copy(init.begin(), init.end(), coord);
		}

		float coord[5];
		inline void SetVertex(const float _x, const float _y, const float _z, const float _tx, const float _ty)
		{
			coord[0] = _x;
			coord[1] = _y;
			coord[2] = _z;
			coord[3] = _tx;
			coord[4] = _ty;
		}
		inline static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
		inline static const DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
		inline static const DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_UNKNOWN;
	};
}


template<int SIZE, typename meshType = MeshType::Vertex2D>
class Mesh
{
	meshType vertices[SIZE];
public:

	Mesh(){}

	Mesh(const std::initializer_list<meshType>& init)
	{
		std::copy(init.begin(), init.end(), vertices);
	}

	inline meshType& operator[](int i)
	{
		return vertices[i];
	}

	inline const meshType& operator[](int i) const
	{
		return vertices[i];
	}

	inline meshType* GetPointerToVertices()
	{
		return vertices;
	}

	inline int MeshSize() const
	{
		return sizeof(vertices);
	}

	inline int VertexSize() const
	{
		return sizeof(meshType);
	}

	inline static const DXGI_FORMAT POSITION_FORMAT = meshType::POSITION_FORMAT;
	inline static const DXGI_FORMAT TEXTURE_FORMAT = meshType::TEXTURE_FORMAT;
	inline static const DXGI_FORMAT COLOR_FORMAT = meshType::COLOR_FORMAT;
};