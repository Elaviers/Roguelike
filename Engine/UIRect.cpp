#include "UIRect.h"
#include "Engine.h"
#include "GLProgram.h"

void UIRect::OnBoundsChanged()
{
	_transform.SetPosition(Vector3(_x + _w / 2.f, _y + _h / 2.f, _z));
	_transform.SetScale(Vector3(_w, _h, 0.f));

}

void UIRect::Render() const
{
	if (_texture)
		_texture->Bind(0);
	else
		Engine::textureManager->White().Bind(0);

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, _transform.GetTransformationMatrix());
	Engine::modelManager->Plane().model.Render();
}
