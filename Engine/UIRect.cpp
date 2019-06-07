#include "UIRect.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

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
		Engine::Instance().pTextureManager->White().Bind(0);

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, _transform.GetTransformationMatrix());
	Engine::Instance().pModelManager->Plane().Render();
}
