#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/String.hpp>
#include <Windows.h>

namespace StringSelect
{
	String Dialog(HWND parent, const Buffer<String> &strings, const char *title);
}
