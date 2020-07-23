#include "Screen.h"
#include <iostream> 
#include <assert.h>
#include <thread>
#include "Raytracing.h"

using namespace std;

const vec3 vec3::zero(0., 0., 0.);
const vec3 vec3::one(1., 1., 1.);
const vec3 vec3::X(1., 0., 0.);
const vec3 vec3::Y(0., 1., 0.);
const vec3 vec3::Z(0., 0., 1.);

Screen* screen = nullptr;

void test()
{
	Ray ray(vec3(), vec3(0.f, -1.f, 0.f));
	{
		assert(ray.getPosition(0) == vec3());
		assert(ray.getPosition(-1) == vec3(0.f, 1.f, 0.f));
		assert(ray.getPosition(1) == vec3(0.f, -1.f, 0.f));
	}
	Sphere sph(vec3(0.f, 0.f, 0.f), 50., vec3(), vec3(), Material::Diffuse);
	{
		Ray r(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 1.0f));
		float a = sph.intersect(r);
		assert(a == 50.f);
	}
	{
		Ray r(vec3(0.f, 0.f, 100.f), vec3(0.f, 0.f, -1.0f));
		float a = sph.intersect(r);
		assert(a == 50.f);
	}
	{
		Ray r(vec3(0.f, 100.f, 0.f), vec3(0.f, -1.f, 0.0f));
		float a = sph.intersect(r);
		assert(a == 50.f);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) 
{
	test();
	screen = new Screen();
	thread rd(render);

    uint32_t size = SIZE;
	int ret = screen->init(size, size, "PathTracing");
	if (ret < 0) {
		exit(ret);
	}

	while (!screen->isExit()) {
		screen->dispatch();
		for (UINT32 i = 0; i < size; i++)
			for (UINT32 j = 0; j < size; j++)
			{
				screen->setColor(i, j, framebuffer[i][j]);
			}

		screen->update();
		Sleep(1);
	}

	return 0;
}