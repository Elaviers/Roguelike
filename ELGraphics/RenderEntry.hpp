#pragma once
#include "ERenderChannels.hpp"
#include "RenderCommand.hpp"
#include <ELCore/Concepts.hpp>
#include <ELCore/List.hpp>
#include <ELMaths/Vector3.hpp>

class Projection;
class Transform;

class RenderEntry
{
protected:
	struct _Command
	{
		const RenderCommand* command;
		bool bDelete;

		_Command(const RenderCommand* command, bool bDelete) : command(command), bDelete(bDelete) {}
	};

	List<_Command> _commands;

	NewHandler _newHandler;
	DeleteHandler _deleteHandler;

	ERenderChannels _renderChannels;

	template <typename T, typename ...Args>
	requires Concepts::DerivedFrom<T, RenderCommand>
		T& _AddCommand(const Args&... args)
	{
		if (_newHandler.IsCallable())
		{
			byte* mem = _newHandler(sizeof(T));
			T* cmd = new(mem) T(args...);
			_commands.Emplace(cmd, true);
			return *cmd;
		}

		T* cmd = new T(args...);
		_commands.Emplace(cmd, true);
		return *cmd;
	}

public:
	RenderEntry(ERenderChannels renderChannels) : _renderChannels(renderChannels) {}
	RenderEntry(ERenderChannels renderChannels, const NewHandler& newHandler, const DeleteHandler& deleteHandler) :  
		//todo: fix memory leak!!!!!!!!!!!!!!!!!!!!!!!!!
		//_commands(newHandler, deleteHandler), 
		//_newHandler(newHandler),
		//_deleteHandler(deleteHandler),
		_renderChannels(renderChannels) {}
	~RenderEntry();
	
	const ERenderChannels& GetRenderChannels() const { return _renderChannels; }

	void Clear();

	void Render(RenderContext&) const;

	void AddCommand(const RenderCommand& command)
	{
		_commands.Emplace(&command, false);
	}

	//This whole block isn't really neccessary.
	//Originally, _AddCommand was public and I just used that, but type safety is enforced better this way...
	//_AddCommand will need to be exposed if I ever need to define RenderCommands outside of this project though....... :/

	RCMDSetBool& AddSetIsFont(bool isFont) { return _AddCommand<RCMDSetBool>(RCMDSetBool::Type::FONT, isFont); }
	RCMDSetFloat& AddSetLineWidth(float lineWidth) { return _AddCommand<RCMDSetFloat>(RCMDSetFloat::Type::LINE_WIDTH, lineWidth); }
	RCMDSetTransform& AddSetTransform(const Matrix4& matrix) { return _AddCommand<RCMDSetTransform>(matrix); }
	RCMDSetTransform& AddSetTransform(Matrix4&& matrix) { return _AddCommand<RCMDSetTransform>(matrix); }
	RCMDSetUVOffset& AddSetUVOffset(const Vector2& uvOffset) { return _AddCommand<RCMDSetUVOffset>(uvOffset); }
	RCMDSetUVOffset& AddSetUVOffset(Vector2&& uvOffset = Vector2()) { return _AddCommand<RCMDSetUVOffset>(uvOffset); }
	RCMDSetUVScale& AddSetUVScale(const Vector2& uvScale) { return _AddCommand<RCMDSetUVScale>(uvScale); }
	RCMDSetUVScale& AddSetUVScale(Vector2&& uvScale = Vector2(1.f, 1.f)) { return _AddCommand<RCMDSetUVScale>(uvScale); }
	RCMDSetColour& AddSetColour(const Colour& colour = Colour::White) { return _AddCommand<RCMDSetColour>(colour); }
	RCMDSetColour& AddSetColour(const Colour& primary, const Colour& secondary) { return _AddCommand<RCMDSetColour>(primary, secondary); }
	RCMDSetTexture& AddSetTexture(const Texture& texture, uint16 unit) { return _AddCommand<RCMDSetTexture>(texture, unit); }
	RCMDSetTexture& AddSetTextureGL(GLuint tex, uint16 unit) { return _AddCommand<RCMDSetTexture>(tex, unit); }
	RCMDSetTexture& AddSetTexture(RCMDSetTexture::Type type, uint16 unit) { return _AddCommand<RCMDSetTexture>(type, unit); }
	RCMDSetCubemap& AddSetCubemap(const GLCubemap& cubemap, uint16 unit) { return _AddCommand<RCMDSetCubemap>(cubemap, unit); }
	RCMDSetCubemap& AddSetCubemapGL(GLuint tex, uint16 unit) { return _AddCommand<RCMDSetCubemap>(tex, unit); }
	RCMDRenderMesh& AddRenderMesh(const Mesh& mesh) { return _AddCommand<RCMDRenderMesh>(mesh); }
	RCMDSetSkinningMatrices& AddSetSkinningMatrices(const Buffer<Matrix4>& matrices) { return _AddCommand<RCMDSetSkinningMatrices>(matrices); }
	RCMDSetSkinningMatrices& AddSetSkinningMatrices(const Buffer<Matrix4>* matrices) { return _AddCommand<RCMDSetSkinningMatrices>(matrices); }
	RCMDLight& AddLight(const Vector3& position, const Vector3& colour, float radius) { return _AddCommand<RCMDLight>(position, colour, radius); }

	void AddLine(const Vector3& a, const Vector3& b);
	void AddBox(const Vector3& p1, const Vector3& p2);
	void AddRing(const Vector3& centre, const Vector3& nomal, float radius, int segments);
	void AddGrid(const Transform& cameraTransform, const Projection& cameraProjection, EDirection plane, float gap, float limit, float offset);
};
