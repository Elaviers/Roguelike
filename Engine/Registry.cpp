#include "Registry.h"


#include "Camera.h"
#include "Light.h"
#include "Renderable.h"

#define QUICKREGISTER(TYPE) this->RegisterObjectClass<TYPE>(#TYPE)

void Registry::RegisterEngineObjects()
{
	QUICKREGISTER(Camera);
	QUICKREGISTER(Light);
	QUICKREGISTER(Renderable);

}
