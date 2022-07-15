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
	static engine::DxResPtr<ID3D11Buffer> vertexBuffer;
	engine::DxResPtr<ID3D11VertexShader> m_vertexShader;
	engine::DxResPtr<ID3D11PixelShader> m_pixelShader;
	engine::DxResPtr<ID3DBlob> m_vertexShaderBlob;
	engine::DxResPtr<ID3DBlob> m_pixelShaderBlob;
	engine::DxResPtr<ID3D11InputLayout> m_inputLayout;

	engine::DxResPtr<ID3D11Buffer> m_transformMatrixBuffer;
	static Mesh<36, MeshType::TexturedVertex3D> m_mesh;

	void CreateShaders();
	void CreateInputLayout();
	void CreateConstantBuffer();
	void Initialize();
	bool math_intersection(math::Intersection& nearest, const ray& r) const;
	Transform m_transform;

	std::string m_samplerStateKey;
	const std::string* m_samplerStateKeyPtr = &engine::TextureManager::instance().GetGlobalSamplerStateKey();
	std::string m_textureKey = "brick";
public:
	Cube(){}

	Cube(float size);

	Cube(const Vec3& scale);

	static void CreateMesh();

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
	const Vec3& position() const { return m_transform.position; }

	inline void SetSamplerState(const std::string& _samplerStateKey) 
	{ 
		m_samplerStateKey = _samplerStateKey;
		m_samplerStateKeyPtr = &m_samplerStateKey;

	}
	inline void SetTexture(const std::string& _textureKey) { m_textureKey = _textureKey; }

	void UpdateTransformMatrixBuffer();
};