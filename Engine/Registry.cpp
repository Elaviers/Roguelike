#include "Registry.h"
#include "Brush2D.h"
#include "Brush3D.h"
#include "Camera.h"
#include "Light.h"
#include "Renderable.h"

#define QUICKREGISTER(TYPE) this->RegisterObjectClass<TYPE>(#TYPE)

void Registry::RegisterEngineObjects()
{
	QUICKREGISTER(Camera);
	QUICKREGISTER(Renderable);
	QUICKREGISTER(Brush2D);
	QUICKREGISTER(Brush3D);
	QUICKREGISTER(Light);

}
