#pragma once
// math : vector & sphere
#include"Math.h"
class Material
{
public:
	color Albedo;
	color Emissive;
	float Roughness;
	float Metallic;

	Material() = default;
};

class Sphere 
{
public:
	vec3 O;
	float R;
	Material* mtl;

	Sphere() = default;
	Sphere(const vec3& o, float r, Material* m) : O(o), R(r), mtl(m) {}
};

class Ray 
{
public:
	vec3 O;
	vec3 D;
	Ray() = default;
	Ray(const vec3& o, const vec3& d) : O(o), D(d) {}
};

class Intersect
{
public:
	const Ray* ray = nullptr;
	const Sphere* sphere = nullptr;

	bool hit = false;

	float t = 0.f;
	vec3 pos;
	vec3 nor;
	Ray* reflectRay;
	Ray* refractRay;

	Intersect() = default;
};

Intersect intersect(const Ray& r, const Sphere& s)
{
	// Ray	  : P(t) = O + Dt
	// Sphere : |P - O| = R
	// => | Or + Dt - Os | = R
	// => ( Dt + Or - Os )^2 = R^2 , let O' = Or - Os
	// => (Dt)^2 + Dt*O' + O'^2 = R^2, let a = DO'/(2*D^2)
	// => (t+a)^2 = a^2 + (R^2-O'^2) / D^2

	vec3 O = r.O - s.O;
	float a = dot(r.D, O) / (2 * dot(r.D, r.D));
	float rhs = a * a + (s.R * s.R - dot(O, O)) / dot(r.D, r.D);

	Intersect i; 
	i.ray = &r; 
	i.sphere = &s;

	if (rhs < 0)
	{
		return i;
	}
	else
	{
		float sqrtrhs = sqrt(rhs);
		i.t = sqrtrhs > a ? sqrtrhs - a : sqrtrhs + a;
	}

	i.pos = r.D + r.O * i.t;
	i.nor = normalize(i.pos - s.O);

	// S.mtl =>
	i.reflectRay = reflect(r.D, i.nor);
	i.refractRay = refract(r.D, i.nor, s.);
}