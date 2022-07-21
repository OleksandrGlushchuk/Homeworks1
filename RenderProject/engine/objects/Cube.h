#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"
#include "../math/Transform.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/Intersection.h"
#include "../objects/ObjRef.h"
#include "../math/Triangle.h"

class Cube
{
	static engine::DxResPtr<ID3D11VertexShader> s_vertexShader;
	static engine::DxResPtr<ID3D11PixelShader> s_pixelShader;
	static engine::DxResPtr<ID3DBlob> s_vertexShaderBlob;
	static engine::DxResPtr<ID3DBlob> s_pixelShaderBlob;

	static engine::DxResPtr<ID3D11Buffer> s_vertexBuffer;
	static engine::DxResPtr<ID3D11InputLayout> s_inputLayout;
	static Mesh<36, MeshType::TexturedVertex3D> s_mesh;

	engine::DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	engine::DxResPtr<ID3D11Buffer> m_transformMatrixBuffer;


	void CreateConstantBuffer();
	bool math_intersection(math::Intersection& nearest, const ray& r) const;

	static void CreateVertexBuffer();
	static void CreateInputLayout();
	static void InitShaders();
	Transform m_transform;
	static bool cubes_textures_initialized;
public:

	Cube(){}

	Cube(float size);

	Cube(const Vec3& scale);

	static void Init();
	static void InitTextures();

	void Rotate(const Quaternion& q);

	void Rotate(const Angles& angles, const Vec3& _right, const Vec3& _top, const Vec3& _forward);

	void Translate(const Vec3& dv);

	void SetPosition(const Vec3& pos);

	void TranslateRelative(const Vec3& dv);

	void Scale(const Vec3& scale);

	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest);

	void Draw();

	const Vec3& right() 	const { return m_transform.right(); }
	const Vec3& top() 		const { return m_transform.top(); }
	const Vec3& forward() 	const { return m_transform.forward(); }
	const Vec3& position()  const { return m_transform.position; }

	inline void SetTexture(const std::string& _textureKey) 
	{
		m_shaderResourceView.release();
		m_shaderResourceView = engine::TextureManager::instance().GetTexture(_textureKey);
	}

	void UpdateTransformMatrixBuffer();
};