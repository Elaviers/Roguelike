#include "Material.hpp"
#include "GL.hpp"

void Material::FromString(const String &data)
{
	Buffer<String> lines = data.ToLower().Split("\r\n");

	for (size_t i = 0; i < lines.GetSize(); ++i)
		_cvars.HandleCommand(lines[i]);
}
