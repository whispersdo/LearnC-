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
	vec3 getPosition(double t) const { return O + D * t; }
};

class Material
{
public:
	enum Type { Reflect, Refract, Diffuse, };
	vec3 E;
	vec3 A;
	Type t;

	Material(const vec3& e, const vec3& a, const Type& t_) : E(e), A(a), t(t_) {};
};

class Sphere
{
public:
	vec3 O;
	double R;
	Material m;

	Sphere() = default;
	Sphere(const vec3& o, double r, Material m_) : O(o), R(r), m(m_) { }
	Sphere(const vec3& o, double r, const vec3& mE, const vec3& mA, const Material::Type& t) : O(o), R(r), m(mE, mA, t) { }

	vec3 getNormal(const vec3& pos) const { return normalize(pos - O);	}

	double intersect(const Ray& r) const
	{
		// Ray	  : P(t) = O + Dt
		// Sphere : |P - O| = R
		// => | Or + Dt - Os | = R
		// => ( Dt + Or - Os )^2 = R^2 , let O' = Or - Os
		// => (Dt)^2 + 2*Dt*O' + O'^2 = R^2, let a = DO'
		// => (t + DO')^2  = (RR - O'O') + (DO')^2

		vec3 Op = O - r.O;
		double a = dot(r.D, Op);
		double rhs = a * a + (R * R - dot(Op, Op));

		if (rhs >= 0)
		{
			double sqrtrhs = sqrt(rhs);
			return a > sqrtrhs ? a - sqrtrhs : a + sqrtrhs;
		}

		return -1.;
	}
};

Sphere sen[] = { //Scene: position, radius, emission, color, material
	Sphere(vec3(0,0,1e3 + 99),		1e3,	vec3(10,10,10),	vec3(),				Material::Diffuse),//Lite
	Sphere(vec3(-30,0,-30),			30,		vec3(),			vec3(1,1,1) * .999,	Material::Reflect),//Mirr
	Sphere(vec3(50,-50,-30),		30,		vec3(),			vec3(1,1,1) * .999,	Material::Refract),//Glas
	Sphere(vec3(-1e5-100,0,0),		1e5,	vec3(),			vec3(.75,.25,.25),	Material::Diffuse),//Left
	Sphere(vec3( 1e5+100,0,0),		1e5,	vec3(),			vec3(.25,.25,.75),	Material::Diffuse),//Rght
	Sphere(vec3(0, -1e5-200,0),		1e5,	vec3(),			vec3(.5,.5,.5),		Material::Diffuse),//Back
	Sphere(vec3(0,1e5+300,0),		1e5,	vec3(),			vec3(),				Material::Diffuse),//Frnt
	Sphere(vec3(0, 0, -1e5-100),	1e5,	vec3(),			vec3(.75,.75,.75),	Material::Diffuse),//Botm
	Sphere(vec3(0, 0, 1e5+100),		1e5,	vec3(),			vec3(.75,.75,.75),	Material::Diffuse),//Top
};

constexpr size_t obj_num = sizeof(sen) / sizeof(Sphere);

bool intersect(const Ray& r, double& t, int& id)
{
	id = -1;
	for (size_t i = 0; i < obj_num; i++)
	{
		double d = sen[i].intersect(r);
		if (d > .0 && d < t)
		{
			t = d; 
			id = i;
		}
	}

	return id != -1;
}

vec3 radiance(const Ray& r, int depth)
{
	double t = 1e20;			// distance to intersection
	int id = -1;				// id of intersected object
	if (!intersect(r, t, id))	// if miss, return black
		return vec3::zero;

	const Sphere& obj = sen[id];        // the hit object

	vec3 x = r.getPosition(t);
	vec3 n = normalize(x - obj.O);
	vec3 nl = dot(n, r.D) < 0. ? n : -n;
	vec3 f = obj.m.A;

	double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl

	if (++depth > 5)
	{
		if (randx() < p)
			f = f * (1 / p);
		else
			return obj.m.E; //R.R.
	}

	if (depth > 100) 
		return obj.m.E;

	if (obj.m.t == Material::Diffuse) // Ideal DIFFUSE reflection 
	{
		float theta = randx() * 0.5 * PI;
		float phi = randx() * 2. * PI;

		Ray ref(x, spheredir(n, theta, phi));

		return obj.m.E + f * radiance(ref, depth);
	}
	else if (obj.m.t == Material::Reflect) // Ideal SPECULAR reflection
	{
		return obj.m.E + f * radiance(Ray(x, reflect(r.D, n)), depth);
	}
	else // Ideal dielectric REFRACTION
	{
		Ray reflRay(x, reflect(r.D, n));    
		bool into = dot(n, nl) > 0;                // Ray from outside going in?
		double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = dot(r.D, nl), cos2t;
		if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)    // Total internal reflection
			return obj.m.E + f * radiance(reflRay, depth);

		vec3 tdir = normalize(r.D * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))));
		double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : dot(tdir, n));
		double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
		return obj.m.E + f * (depth > 2 ? (randx() < P ?   // Russian roulette
			radiance(reflRay, depth) * RP : radiance(Ray(x, tdir), depth) * TP) :
			radiance(reflRay, depth) * Re + radiance(Ray(x, tdir), depth) * Tr);
	}
};

class Camera
{
public:
	vec3 Pos, Dir;
	double Fov;
	vec3 right, up;
	double fovscale;

	Camera() = default;
	Camera(const vec3& pos, const vec3& dir, double fov)
		: Pos(pos), Dir(dir), Fov(fov) {}

	void init()
	{
		right = normalize(cross(Dir, vec3::Z));
		up = normalize(cross(right, Dir));
		fovscale = tan(0.5 * Fov * PI / 180.) * 2.;
	}

	Ray generateRay(double x, double y)
	{
		vec3 r = right * ((x - 0.5) * fovscale);
		vec3 u = up * ((0.5 - y) * fovscale);
		return Ray(Pos, normalize(Dir + r + u));
	}
};

#define SIZE 1024
uint32_t framebuffer[SIZE][SIZE];
Camera cam(vec3(0,250,0), normalize(vec3(0, -1, 0)), 45.);

void render()
{
	size_t t = time(0);
	srand(t);

	cam.init();

	double offset = 1.0f / SIZE;
	static const size_t samplecount = 1;

	for (size_t i = 0; i < SIZE; i++)
	{
		for (size_t j = 0; j < SIZE; j++)
		{
			vec3 sum;
			for (size_t s = 0; s < samplecount; s++)
			{
				Ray r = cam.generateRay(offset * (i + randx(-.25, .25)), offset * (j + randx(-.25, .25)));
				sum = sum + radiance(r, 0);
			}

			framebuffer[i][j] = (uint32_t) (sum / (float) samplecount);
		}
	}

}
