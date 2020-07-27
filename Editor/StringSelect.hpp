#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/String.hpp>
#include <Windows.h>

namespace StringSelect
{
	String Dialog(HWND parent, const Buffer<String> &strings, const char *title);
}
