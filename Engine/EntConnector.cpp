#include "EntConnector.hpp"
#include "CollisionBox.hpp"
#include "Colour.hpp"
#include "DrawUtils.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

Collider EntConnector::COLLIDER = Collider(ECollisionChannels::EDITOR, CollisionBox(Transform(Vector3(), Rotation(), Vector3(.5f, .5f, 0.f))));

void EntConnector::Render(ERenderChannels channels) const {
	static const Mat4 planeT = Matrix::RotationY(180.f);

	if (Engine::Instance().pModelManager && (channels & ERenderChannels::UNLIT))
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour::Green);
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, planeT * GetTransformationMatrix());
		Engine::Instance().pTextureManager->White()->Bind(0);
		Engine::Instance().pModelManager->Plane()->Render();

		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour::Red);
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		Engine::Instance().pModelManager->Plane()->Render();
	}
}
