#include "Skeleton.hpp"
#include "GLProgram.hpp"

void Skeleton::ToShader() const
{
	if (_skinningMatrices.GetSize())
		glUniformMatrix4fv(GLProgram::Current().GetUniformLocation(DefaultUniformVars::mat4aBones), 
			(GLsizei)_skinningMatrices.GetSize(), GL_FALSE, (float*)_skinningMatrices.Data());
}
