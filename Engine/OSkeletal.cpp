#include "OSkeletal.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/AnimationManager.hpp>
#include <ELGraphics/Mesh_Skeletal.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

PropertyCollection& OSkeletal::GetProperties()
{
	static PropertyCollection properties(ORenderable::GetProperties());

	DO_ONCE_BEGIN;
	properties.AddCommand("Play", MemberCommandPtr<OSkeletal>(&OSkeletal::_CMD_PlayAnimation));
	DO_ONCE_END;

	return properties;
}

void OSkeletal::_CMD_PlayAnimation(const Array<String>& args, const Context& ctx)
{
	if (args.GetSize())
	{
		AnimationManager* animManager = ctx.GetPtr<AnimationManager>();
		SharedPointer<Animation> anim = animManager->Get(args[0], ctx);

		if (anim)
			PlayAnimation(anim);
	}
}

void OSkeletal::_OnMeshChanged()
{
	const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh().Ptr());
	if (skeletal) _skinningMatrices.SetSize(skeletal->skeleton.GetJointCount());
}

void OSkeletal::PlayAnimation(const SharedPointer<const Animation> &anim)
{
	_animation = anim;
	_currentTime = 0.f;
}

void OSkeletal::Update(float deltaTime)
{
	_currentTime += deltaTime;

	if (_animation)
	{
		const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh().Ptr());
		if (skeletal)
			_animation->Evaluate(_skinningMatrices, skeletal->skeleton, _currentTime);
	}
}

void OSkeletal::Render(RenderQueue& q) const
{
	if (_model)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE);
		
		if (_material)
		{
			_material->Apply(e);
		}

		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddCommand(RCMDSetUVScale::Default());

		e.AddSetTransform(GetAbsoluteTransform().GetMatrix());
		e.AddSetColour(_colour);
		e.AddSetSkinningMatrices(&_skinningMatrices);
		e.AddRenderMesh(*_model->GetMesh());
	}
}
