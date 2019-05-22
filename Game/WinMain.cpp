#include <Windows.h>
#include "Game.hpp"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR commandString, int cmdShow)
{
	Game game;
	game.Run();
	return 0;
}
