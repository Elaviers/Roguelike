#include "Registry.hpp"
#include "ObjectIDS.hpp"
#include "EntBox.hpp"
#include "EntBrush2D.hpp"
#include "EntBrush3D.hpp"
#include "EntConnector.hpp"
#include "EntCamera.hpp"
#include "EntLight.hpp"
#include "EntRenderable.hpp"
#include "EntSprite.hpp"

void Registry::RegisterEngineObjects()
{
	RegisterObjectClass<Entity>		(EntityIDS::Entity,		"Entity");
	RegisterObjectClass<EntRenderable>  (EntityIDS::RENDERABLE,		"Renderable");
	RegisterObjectClass<EntBrush3D>		(EntityIDS::BRUSH,			"Brush");
	RegisterObjectClass<EntBrush2D>		(EntityIDS::PLANE,			"Plane");
	RegisterObjectClass<EntLight>		(EntityIDS::LIGHT,			"Light");
	RegisterObjectClass<EntSprite>		(EntityIDS::SPRITE,			"Sprite");
	RegisterObjectClass<EntConnector>	(EntityIDS::LEVEL_CONNECTOR,"Level Connector");
	RegisterObjectClass<EntBox>			(EntityIDS::BOX,			"Box");
	RegisterObjectClass<EntCamera>		(EntityIDS::CAMERA,			"Camera");
}
