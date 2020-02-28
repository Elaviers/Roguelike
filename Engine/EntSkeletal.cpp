#include "EntSkeletal.hpp"
#include "AnimationManager.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "Mesh_Skeletal.hpp"

PropertyCollection& EntSkeletal::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties = EntRenderable::GetProperties();

	properties.AddCommand("Play", MemberCommandPtr<EntSkeletal>(&EntSkeletal::_CMD_PlayAnimation));
	DO_ONCE_END;

	return properties;
}

void EntSkeletal::_CMD_PlayAnimation(const Buffer<String>& args)
{
	if (args.GetSize())
	{
		SharedPointer<Animation> anim = Engine::Instance().pAnimationManager->Get(args[0]);

		if (anim)
			PlayAnimation(anim);
	}
}

void EntSkeletal::_OnModelChanged()
{
	const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh());
	if (skeletal) _skinningMatrices.SetSize(skeletal->skeleton.GetJointCount());
}

void EntSkeletal::PlayAnimation(const SharedPointer<const Animation> &anim)
{
	_animation = anim;
	_currentTime = 0.f;
}

void EntSkeletal::Update(float deltaTime)
{
	_currentTime += deltaTime;

	if (_animation)
	{
		const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh());
		if (skeletal)
			_animation->Evaluate(_skinningMatrices, skeletal->skeleton, _currentTime);
	}
}

void EntSkeletal::Render(ERenderChannels channels) const
{
	if (_model)
	{
		if (_material)
		{
			if (!(channels & _material->GetRenderChannelss()))
				return;

			_material->Apply();
		}

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
		
		if (_skinningMatrices.GetSize())
			glUniformMatrix4fv(GLProgram::Current().GetUniformLocation(DefaultUniformVars::mat4aBones),
			(GLsizei)_skinningMatrices.GetSize(), GL_FALSE, (float*)_skinningMatrices.Data());

		_model->Render();
	}
}
