#include "render/opengl/OpenglRenderer.h"

namespace render::opengl
{
	bool OpenglRenderer::reloadShaders(OpenglContext& openglContext) {
		assert(0);
		return false;
	}

	OpenglRenderer::OpenglRenderer(OpenglContext& openglContext_)
	    : openglContext(openglContext_),
	      program(openglContext) {
	}

	void OpenglRenderer::addUniform(std::string_view name, render::DataType uniformDataType, uint32_t count) {
	}
}
