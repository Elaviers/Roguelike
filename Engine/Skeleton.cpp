#include "Skeleton.hpp"
#include "GLProgram.hpp"

void Skeleton::UpdateCache()
{
	size_t jointCount = _joints.GetSize();

	if (_transformCache.GetSize() != jointCount)
		_transformCache.SetSize(jointCount);

	for (auto node = _joints.First(); node; node = node->next)
		_transformCache[node->obj.GetID()] = node->obj.localTransform.GetTransformationMatrix();
}

void Skeleton::ToShader() const
{
	if (_transformCache.GetSize())
		glUniformMatrix4fv(GLProgram::Current().GetUniformLocation(DefaultUniformVars::mat4aBones), 
			_transformCache.GetSize(), GL_FALSE, (float*)_transformCache.Data());
}
