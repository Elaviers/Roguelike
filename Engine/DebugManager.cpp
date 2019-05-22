#include "DebugManager.hpp"
#include "DebugObject.hpp"
#include "Font.hpp"
#include "String.hpp"

void DebugManager::Update(float deltaTime)
{
	uint32 i = 0;

	while (i < _worldObjects.GetSize())
	{
		_worldObjects[i]->Update(deltaTime);

		if (_worldObjects[i]->ShouldBeRemoved())
			_worldObjects.RemoveIndex(i);
		else
			i++;
	}

	while (i < _screenObjects.GetSize())
	{
		_worldObjects[i]->Update(deltaTime);

		if (_screenObjects[i]->ShouldBeRemoved())
			_screenObjects.RemoveIndex(i);
		else
			i++;
	}
}

void DebugManager::RenderWorld() const
{
	for (uint32 i = 0; i < _worldObjects.GetSize(); ++i)
		_worldObjects[i]->Render();
}

void DebugManager::RenderScreen() const
{
	for (uint32 i = 0; i < _screenObjects.GetSize(); ++i)
		_screenObjects[i]->Render();
}

#include "DebugString.hpp"

void DebugManager::AddString(const String &string)
{
	if (_font)
	{
		DebugString *obj = new DebugString(*_font, Vector3(0, 0, 0), 32, string, 20.f, Vector4(1.f, 0, 0));
		_screenObjects.Add(obj);
	}
}
