#pragma once
#include "vec3.h"
#include "Material.h"
#include "camera.h"

class Point_Light
{
public:
	float R;
	Vec3 color;
	Vec3 pos;
};

class Spot_Light : public Point_Light
{
public:
	Vec3 direction;
	float cutoff;
};

class Directional_Light
{
public:
	Vec3 direction;
	Vec3 color;
};

inline Vec3 CalculateDirectionalLight(const Directional_Light& light, const Camera& camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 H(camera.position() - nearest_point);
	H.make_unit_vector();
	float NdotL = dot(nearest_normal, light.direction);
	float spec = pow(dot(nearest_normal, H), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo + spec_component);
}

inline Vec3 CalculatePointLight(const Point_Light& light, const Camera &camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 L(light.pos - nearest_point);
	Vec3 V(camera.position() - nearest_point);
	Vec3 H = V + L;
	H.make_unit_vector();
	L.make_unit_vector();
	float D = length(light.pos - nearest_point);
	float NdotL = dot(nearest_normal, L);
	float spec = pow(dot(nearest_normal, H), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo * 1.f / pow(D / light.R, 2) + spec_component);
}