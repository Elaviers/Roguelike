#include "Registry.hpp"
#include "ObjBox.hpp"
#include "ObjBrush2D.hpp"
#include "ObjBrush3D.hpp"
#include "ObjConnector.hpp"
#include "ObjCamera.hpp"
#include "ObjLight.hpp"
#include "ObjRenderable.hpp"

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
