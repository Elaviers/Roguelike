#include "Registry.hpp"
#include "EntityID.hpp"
#include "EntBox.hpp"
#include "EntBrush2D.hpp"
#include "EntBrush3D.hpp"
#include "EntConnector.hpp"
#include "EntCamera.hpp"
#include "EntLight.hpp"
#include "EntRenderable.hpp"
#include "EntSkeletal.hpp"
#include "EntSprite.hpp"

void Registry::RegisterEngineObjects()
{
	RegisterObjectClass<Entity>			("Entity");
	RegisterObjectClass<EntRenderable>  ("Renderable");
	RegisterObjectClass<EntSkeletal>	("Skeletal");
	RegisterObjectClass<EntBrush3D>		("Brush");
	RegisterObjectClass<EntBrush2D>		("Plane");
	RegisterObjectClass<EntLight>		("Light");
	RegisterObjectClass<EntSprite>		("Sprite");
	RegisterObjectClass<EntConnector>	("Level Connector");
	RegisterObjectClass<EntBox>			("Box");
	RegisterObjectClass<EntCamera>		("Camera");
}
