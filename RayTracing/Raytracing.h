#pragma once
// math : vector & sphere
#include"Math.h"

class Ray
{
public:
	vec3 O;
	vec3 D;
	Ray() = default;
	Ray(const vec3& o, const vec3& d) : O(o), D(d) {}
	vec3 getPosition(float t) const { return O + D * t; }
};

class Material
{
public:
	vec3 Albedo;
	vec3 Emissive;
	float Roughness;
	float Metallic;

	float refractive;

	Material() = default;
	vec3 getReflectRay(const vec3& i, const vec3& n) const {
		return reflect(i, n);
	}

	vec3 getRefractRay(const vec3& i, const vec3& n) const {
		return refract(i, n, refractive);
	}
};

class Geo 
{
public:
	Material* mtl;

	Geo(Material* m) : mtl(m) {}

	virtual vec3 getNormal(const vec3& pos) const { return vec3(); }
	virtual float interset(const Ray& r) const { return 0.f; }
};

class Sphere : public Geo
{
public:
	vec3 O;
	float R;

	Sphere() = default;
	Sphere(const vec3& o, float r, Material* m) : O(o), R(r), Geo(m) {}

	virtual vec3 getNormal(const vec3& pos) const override { return normalize(pos - O);	}

	virtual float interset(const Ray& r) const override
	{
		// Ray	  : P(t) = O + Dt
		// Sphere : |P - O| = R
		// => | Or + Dt - Os | = R
		// => ( Dt + Or - Os )^2 = R^2 , let O' = Or - Os
		// => (Dt)^2 + 2*Dt*O' + O'^2 = R^2, let a = DO'
		// => (t + DO')^2  = (RR - O'O') + (DO')^2

		vec3 Op = O - r.O;
		float a = dot(r.D, Op);
		float rhs = a * a + (R * R - dot(Op, Op));

		if (rhs >= 0)
		{
			float sqrtrhs = sqrt(rhs);
			return a > sqrtrhs ? a - sqrtrhs : a + sqrtrhs;
		}

		return -1.f;
	}
};

class Plane : Geo
{
public:
	vec3 N;
	float D;

	Plane() = default;
	Plane(const vec3& n, float d, Material* m) : N(n), D(d), Geo(m) {}

	virtual vec3 getNormal(const vec3& pos) const override { return N; }

	virtual float interset(const Ray& r) const override
	{
		// Ray	 : P(t) = O + Dt
		// Plane : P - Nd ⊥ N => dot(N, P-Nd) = 0 => dot(N,P) = d
		// (O+Dt) * N = d  
		// 

		return -1.f;
	}
};

class Intersect
{
public:
	Ray ray;
	Geo* geo = nullptr;

	float t = 0.f;
	vec3 pos;
	vec3 nor;
	Ray	reflectRay;
	Ray	refractRay;

	Intersect() = default;
	Intersect(const Ray& r, Geo* g, float t_) :ray(r), geo(g), t(t_) {}

	void calc()
	{
		pos = ray.getPosition(t);
		nor = geo->getNormal(pos);
		reflectRay = Ray(pos, geo->mtl->getReflectRay(ray.D, nor));
		refractRay = Ray(pos, geo->mtl->getRefractRay(ray.D, nor));
	}
};


class Camera
{
public:
	vec3 Pos, Dir;
	float Fov;
	vec3 right, up;
	float fovscale;

	Camera() {};

	Camera(const vec3& pos, const vec3& dir, float fov)
		: Pos(pos), Dir(dir), Fov(fov) {}

	void init()
	{
		right = normalize(cross(Dir, vec3(0.f,0.f,1.f)));
		up = normalize(cross(right, Dir));
		fovscale = tan(0.5f * Fov * PI / 180.0f) * 2.0f;
	}

	Ray generateRay(float x, float y)
	{
		vec3 r = right * ((x - 0.5f) * fovscale);
		vec3 u = up * ((0.5f - y) * fovscale);
		return Ray(Pos, normalize(Dir + r + u));
	}
};

#define GeoCount 8
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

class Scene {
public:
	Material*	Mtls[GeoCount];
	Geo*		Objs[GeoCount];
	vec4		framebuffer2[WINDOW_WIDTH][WINDOW_HEIGHT];
	uint32_t	framebuffer[WINDOW_WIDTH][WINDOW_HEIGHT];
	Camera		Cam;

	Scene() 
	{
		Mtls[0] = new Material(); 
		Mtls[0]->Emissive = vec3(10.0f,1.0f,1.0f);
		Mtls[0]->Albedo = vec3(0.0f,0.0f,0.0f);
		Mtls[1] = new Material();
		Mtls[1]->Emissive = vec3(0.0f, 0.0f, 0.0f);
		Mtls[1]->Albedo = vec3(0.5f, 0.5f, 0.5f);
		Mtls[1]->Roughness = 1.0f;

		/*Mtls[2] = new Material();
		Mtls[3] = new Material();*/

		Objs[0] = new Sphere(vec3(0.f, 0.f, 1e3f+99.f), 1e3f, Mtls[0]); // Light
		Objs[1] = new Sphere(vec3(0.f, 0.f, 1e5f + 100.f), 1e5f, Mtls[1]); // 上
		Objs[2] = new Sphere(vec3(0.f, 0.f, 1e5f - 100.f), 1e5f, Mtls[1]); // 下
		Objs[3] = new Sphere(vec3(1e5f - 100.f, 0.f, 0.f), 1e5f, Mtls[1]); // 左
		Objs[4] = new Sphere(vec3(1e5f + 100.f, 0.f, 0.f), 1e5f, Mtls[1]); // 右
		Objs[5] = new Sphere(vec3(0.f, 1e5f + 300.f, 0.f), 1e5f, Mtls[1]); // 前
		Objs[6] = new Sphere(vec3(0.f, 1e5f - 300.f, 0.f), 1e5f, Mtls[1]); // 后
		Objs[7] = new Sphere(vec3(0.f, 0.f, 30.f), 10.f, Mtls[1]); // 物体1
		Objs[7] = new Sphere(vec3(0.f, 0.f, 30.f), 10.f, Mtls[1]); // 物体2

		Cam = Camera(vec3(0.f, 299.f, 50.f), vec3(0.f, -1.f, 0.f), 45);
		Cam.init();

		for (size_t i = 0; i < WINDOW_WIDTH; i++)
		{
			for (size_t j = 0; j < WINDOW_HEIGHT; j++)
				framebuffer[i][j] = 0;
		}
	};

	void intersect(const Ray& r, Geo*& geo, float& t)
	{
		for (auto o : Objs)
		{
			float t1 = o->interset(r);
			if (t1 < t && t1 >= 0)
			{
				t = t1;
				geo = o; 
			}
		}
	}

	vec3 radiance(const Ray& r, uint32_t depth)
	{
		depth ++;

		if (depth > 5)
			return vec3();

		Geo* geo = nullptr;
		float t = 1e30f;
		intersect(r, geo, t);
		if (geo == nullptr) // 没东西
			return vec3();

		Intersect it(r, geo, t);
		it.calc();

		//return spheredir(it.nor, 0.0f, 3.14f);
		if (geo->mtl->Albedo == vec3(0.0f, 0.0f, 0.0f))
			return geo->mtl->Emissive;

		float rough = geo->mtl->Roughness;
		if (rough == 0.0f)
			return geo->mtl->Emissive + geo->mtl->Albedo * radiance(it.reflectRay, depth);

		float theta = rand01() * rough;
		float phi = rand01();
		
		//return spheredir(it.nor, 1.f, 1.f);
		return normalize(it.nor) * -1.f;

		Ray ref(it.pos, spheredir(it.nor, theta, phi));
		return geo->mtl->Emissive + geo->mtl->Albedo * max(0.f, dot(ref.D, it.nor)) * radiance(ref, depth);

		return vec3();
	}

	void render()
	{
		vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		float offsetx = 1.0f / WINDOW_WIDTH;
		float offsety = 1.0f / WINDOW_HEIGHT;
		static const size_t samplecount = 1;

		for (size_t i = 0; i < WINDOW_WIDTH; i++)
		{
			for (size_t j = 0; j < WINDOW_HEIGHT; j++)
			{
				vec3 sum(.0f, .0f, .0f);
				for (size_t s = 0; s < samplecount; s++)
				{
					Ray r = Cam.generateRay(offsetx * (i + rand01() * 0.5f), offsety * (j + rand01() * 0.5f));
					sum = sum + radiance(r, 0);
				}

				framebuffer[i][j] = (uint32_t) (sum / (float) samplecount);
			}
		}

	}

};
