#include "Registry.h"
#include "ObjBox.h"
#include "ObjBrush2D.h"
#include "ObjBrush3D.h"
#include "ObjConnector.h"
#include "ObjCamera.h"
#include "ObjLight.h"
#include "ObjRenderable.h"

void Registry::RegisterEngineObjects()
{
	RegisterObjectClass<ObjBox>			(101,	"Box");
	RegisterObjectClass<ObjBrush2D>		(3,		"Plane");
	RegisterObjectClass<ObjBrush3D>		(2,		"Brush");
	RegisterObjectClass<ObjConnector>	(100,	"Level Connector");
	RegisterObjectClass<ObjCamera>		(102,	"Camera");
	RegisterObjectClass<ObjLight>		(4,		"Light");
	RegisterObjectClass<ObjRenderable>	(1,		"Renderable");
}
