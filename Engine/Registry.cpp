#include "Registry.hpp"
#include "ObjectIDS.hpp"
#include "ObjBox.hpp"
#include "ObjBrush2D.hpp"
#include "ObjBrush3D.hpp"
#include "ObjConnector.hpp"
#include "ObjCamera.hpp"
#include "ObjLight.hpp"
#include "ObjRenderable.hpp"
#include "ObjSprite.hpp"

void Registry::RegisterEngineObjects()
{
	RegisterObjectClass<GameObject>		(ObjectIDS::GAMEOBJECT,		"Gameobject");
	RegisterObjectClass<ObjRenderable>  (ObjectIDS::RENDERABLE,		"Renderable");
	RegisterObjectClass<ObjBrush3D>		(ObjectIDS::BRUSH,			"Brush");
	RegisterObjectClass<ObjBrush2D>		(ObjectIDS::PLANE,			"Plane");
	RegisterObjectClass<ObjLight>		(ObjectIDS::LIGHT,			"Light");
	RegisterObjectClass<ObjSprite>		(ObjectIDS::SPRITE,			"Sprite");
	RegisterObjectClass<ObjConnector>	(ObjectIDS::LEVEL_CONNECTOR,"Level Connector");
	RegisterObjectClass<ObjBox>			(ObjectIDS::BOX,			"Box");
	RegisterObjectClass<ObjCamera>		(ObjectIDS::CAMERA,			"Camera");
}
