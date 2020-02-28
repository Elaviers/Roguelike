#include "UIRect.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

void UIRect::_OnBoundsChanged()
{
	_transform.SetPosition(Vector3(_absoluteBounds.x + _absoluteBounds.w / 2.f, _absoluteBounds.y + _absoluteBounds.h / 2.f, _z));
	_transform.SetScale(Vector3(_absoluteBounds.w, _absoluteBounds.h, 0.f));
}

void UIRect::Render() const
{
	if (_texture)
		_texture->Bind(0);
	else
		Engine::Instance().pTextureManager->White()->Bind(0);

	_colour.ApplyToShader();
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, _transform.GetTransformationMatrix());
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	Engine::Instance().pModelManager->Plane()->Render();
	GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, false);
}
