#include "EntSkeletal.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/AnimationManager.hpp>
#include <ELGraphics/Mesh_Skeletal.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

PropertyCollection& EntSkeletal::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties = EntRenderable::GetProperties();

	properties.AddCommand("Play", MemberCommandPtr<EntSkeletal>(&EntSkeletal::_CMD_PlayAnimation));
	DO_ONCE_END;

	return properties;
}

void EntSkeletal::_CMD_PlayAnimation(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize())
	{
		AnimationManager* animManager = ctx.GetPtr<AnimationManager>();
		SharedPointer<Animation> anim = animManager->Get(args[0], ctx);

		if (anim)
			PlayAnimation(anim);
	}
}

void EntSkeletal::_OnMeshChanged()
{
	const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh().Ptr());
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
		const Mesh_Skeletal* skeletal = dynamic_cast<const Mesh_Skeletal*>(_model->GetMesh().Ptr());
		if (skeletal)
			_animation->Evaluate(_skinningMatrices, skeletal->skeleton, _currentTime);
	}
}

void EntSkeletal::Render(RenderQueue& q) const
{
	if (_model)
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::SURFACE);
		
		if (_material)
		{
			_material->Apply(e);
		}

		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddCommand(RCMDSetUVScale::Default());

		e.AddSetTransform(GetTransformationMatrix());
		e.AddSetColour(_colour);
		e.AddSetSkinningMatrices(&_skinningMatrices);
		e.AddRenderMesh(*_model->GetMesh());
	}
}
