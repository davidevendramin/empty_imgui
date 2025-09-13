#include "GUI.h"
#include "pch.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	GUI gui = GUI();
	gui.run();

	return 0;
}