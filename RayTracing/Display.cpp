#include "Screen.h"
#include "Raytracing.h"
#include <assert.h>

Screen* screen = nullptr;
#include <iostream> 
using namespace std;

void test()
{
	// Math
	vec3 X =	 { 1.0f, 0.0f, 0.0f };
	vec3 Y = vec3( 0.0f, 1.0f, 0.0f );
	vec3 Z		 { 0.0f, 0.0f, 1.0f };

	{
		vec3 transx = spheredir(X, 0.0f, 0.0f);
		vec3 transy = spheredir(Y, 0.0f, 0.0f);
		vec3 transz = spheredir(Z, 0.0f, 0.0f);
		vec3 transc = spheredir(vec3(.5f, .25f, .4f), 0.0f, 0.0f);
	}

	assert(spheredir(vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f) == vec3(0.0f, 1.0f, 0.0f));

	Ray ray(vec3(), vec3(0.f, -1.f, 0.f));
	{
		assert(ray.getPosition(0) == vec3());
		assert(ray.getPosition(-1) == vec3(0.f, 1.f, 0.f));
		assert(ray.getPosition(1) == vec3(0.f, -1.f, 0.f));
	}

	Sphere sph(vec3(0.f, 0.f, 0.f), 50, nullptr);
	{
		Ray r(vec3(0.f,0.f,0.f), vec3(0.f, 0.f, 1.0f));
		float a = sph.interset(r);
		assert(a == 50.f);
	}
	{
		Ray r(vec3(0.f, 0.f, 100.f), vec3(0.f, 0.f, -1.0f));
		float a = sph.interset(r);
		assert(a == 50.f);
	}
	{
		Sphere sph(vec3(0.f, 0.f, 50.f), 30, nullptr);
		Ray r(vec3(0.f, 50.f, 50.f), vec3(0.f, -1.0f, 0.f));
		float a = sph.interset(r);
		assert(a == 20.f);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) 
{
	test();

	screen = new Screen();
	Scene* sen = new Scene;
	sen->render();

    uint32_t size = 512;
	int ret = screen->init(size, size, "Raytracing");
	if (ret < 0) {
		exit(ret);
	}

	while (!screen->isExit()) {
		screen->dispatch();
		for (UINT32 i = 0; i < size; i++)
			for (UINT32 j = 0; j < size; j++)
			{
				screen->setColor(i, j, sen->framebuffer[i][j]);
			}

		screen->update();
		Sleep(1);
	}

	return 0;
}