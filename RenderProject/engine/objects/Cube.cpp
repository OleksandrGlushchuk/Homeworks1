#include "Cube.h"

std::shared_ptr<Model> Cube::s_model;

bool Cube::math_intersection(math::Intersection& nearest, const ray& r) const
{
	Vec3 v0, v1, v2;
	ray rr = r;
	auto& transform = engine::MeshSystem::instance().Get_TransformMatrix(ID);
	auto& transformInv = engine::MeshSystem::instance().Get_TransformInvMatrix(ID);
	rr.direction.mult(transformInv, 0);
	rr.origin.mult(transformInv, 1);
	int k = 0;
	for (int i = 0; i < 12; ++i)
	{

		v0 = s_model->m_meshes[0].vertices[s_model->m_meshes[0].triangles[i].indices[0]].position;
		v1 = s_model->m_meshes[0].vertices[s_model->m_meshes[0].triangles[i].indices[1]].position;
		v2 = s_model->m_meshes[0].vertices[s_model->m_meshes[0].triangles[i].indices[2]].position;
		
		math::Triangle tr(v0, v1, v2, Vec3(Vec3::cross(v1 - v0, v2 - v0)));
		if (tr.intersection(nearest, rr))
		{
			k++;
		}
	}
	if (k > 0)
	{
		nearest.normal.mult(transform, 0);
		nearest.normal.normalize();
		nearest.point.mult(transform, 1);
		return true;
	}
	return false;
}

bool Cube::intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest)
{
	bool found = math_intersection(outNearest, _ray);
	if (found)
	{
		outRef.type = ObjRef::IntersectedType::CUBE;
		outRef.object = this;
	}
	return found;
}


void Cube::Init()
{
	s_model = std::make_shared<Model>();
	auto& model = *s_model;
	model.name = "Cube";
	model.box = {};
	
	model.m_meshes.resize(1);
	model.m_meshRanges.resize(1);

	
	auto& meshRange = model.m_meshRanges[0];
	meshRange.box = {};
	meshRange.indexNum = 36;
	meshRange.indexOffset = 0;
	meshRange.vertexOffset = 0;
	meshRange.vertexNum = 16;

	auto& mesh = model.m_meshes[0];
	Matr<4> meshToModelMatrix = Matr<4>::identity();
	mesh.m_constantBuffer.Init(D3D11_USAGE_IMMUTABLE, 0, &meshToModelMatrix);
	mesh.name = "CubeMesh";
	mesh.vertices.resize(16);
	mesh.triangles.resize(12);

	mesh.vertices[0].position = Vec3(-0.5f, -0.5f, -0.5f);
	mesh.vertices[1].position = Vec3(-0.5f, 0.5f, -0.5f);
	mesh.vertices[2].position = Vec3(0.5f, 0.5f, -0.5f);
	mesh.vertices[3].position = Vec3(0.5f, -0.5f, -0.5f);

	mesh.vertices[4].position = Vec3(0.5f, -0.5f, 0.5f);
	mesh.vertices[5].position = Vec3(0.5f, 0.5f, 0.5f);
	mesh.vertices[6].position = Vec3(-0.5f, 0.5f, 0.5f);
	mesh.vertices[7].position = Vec3(-0.5f, -0.5f, 0.5f);

	mesh.vertices[8].position = mesh.vertices[1].position;
	mesh.vertices[9].position = mesh.vertices[6].position;
	mesh.vertices[10].position = mesh.vertices[5].position;
	mesh.vertices[11].position = mesh.vertices[2].position;

	mesh.vertices[12].position = mesh.vertices[7].position;
	mesh.vertices[13].position = mesh.vertices[0].position;
	mesh.vertices[14].position = mesh.vertices[3].position;
	mesh.vertices[15].position = mesh.vertices[4].position;


	mesh.vertices[0].textureCoords = Vec2(0, 1);
	mesh.vertices[1].textureCoords = Vec2(0, 0);
	mesh.vertices[2].textureCoords = Vec2(1, 0);
	mesh.vertices[3].textureCoords = Vec2(1, 1);

	mesh.vertices[4].textureCoords = Vec2(0, 1);
	mesh.vertices[5].textureCoords = Vec2(0, 0);
	mesh.vertices[6].textureCoords = Vec2(1, 0);
	mesh.vertices[7].textureCoords = Vec2(1, 1);

	mesh.vertices[8].textureCoords = Vec2(0, 1);
	mesh.vertices[9].textureCoords = Vec2(0, 0);
	mesh.vertices[10].textureCoords = Vec2(1, 0);
	mesh.vertices[11].textureCoords = Vec2(1, 1);

	mesh.vertices[12].textureCoords = Vec2(0, 1);
	mesh.vertices[13].textureCoords = Vec2(0, 0);
	mesh.vertices[14].textureCoords = Vec2(1, 0);
	mesh.vertices[15].textureCoords = Vec2(1, 1);


	mesh.triangles[0] = TriangleIndices(0, 1, 2);
	mesh.triangles[1] = TriangleIndices(2, 3, 0);

	mesh.triangles[2] = TriangleIndices(8, 9, 10);
	mesh.triangles[3] = TriangleIndices(10, 11, 8);

	mesh.triangles[4] = TriangleIndices(4, 5, 6);
	mesh.triangles[5] = TriangleIndices(6, 7, 4);

	mesh.triangles[6] = TriangleIndices(12, 13, 14);
	mesh.triangles[7] = TriangleIndices(14, 15, 12);

	mesh.triangles[8] = TriangleIndices(7, 6, 1);
	mesh.triangles[9] = TriangleIndices(1, 0, 7);

	mesh.triangles[10] = TriangleIndices(3, 2, 5);
	mesh.triangles[11] = TriangleIndices(5, 4, 3);

	model.m_indexBuffer.Init(mesh.triangles);
	model.m_vertexBuffer.Init(mesh.vertices);
}
