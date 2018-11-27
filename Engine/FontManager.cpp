#include "FontManager.h"
#include "IO.h"

constexpr const char *extension = ".txt";

const Font* FontManager::GetFont(const String &nameIn)
{
	String name = nameIn.ToLower();
	Font *font = _map.Find(name);

	if (font) return font;
	else if (name.GetLength() > 0)
	{
		String fs = IO::ReadFileString((_rootPath + name + extension).GetData());

		if (fs.GetLength())
		{
			Font &newFont = _map[name];
			newFont.FromString(fs);
			return &newFont;
		}
	}

	return nullptr;
}
