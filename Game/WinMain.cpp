#include <Windows.h>
#include "Game.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR commandString, int cmdShow)
{
	Game game;
	game.Run();
	return 0;
}
