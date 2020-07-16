#include "Screen.h"
#include "Raytracing.h"

Screen* screen = NULL;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) 
{
	screen = new Screen();

	vec4 vx{ 0,0,0,1 };
	//cout<<vx.r();

	int ret = screen->init(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracing");
	if (ret < 0) {
		exit(ret);
	}

	while (!screen->isExit()) {
		screen->dispatch();
		for (UINT32 i = 0; i < WINDOW_WIDTH; i++)
			for (UINT32 j = 0; j < WINDOW_HEIGHT; j++)
				screen->setColor(i,j,0xFF000000);

		screen->update();
		Sleep(1);
	}
	return 0;
}