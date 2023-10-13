#include "render/opengl/OpenglRenderer.h"

namespace render::opengl
{
	bool OpenglRenderer::reloadShaders(OpenglContext& openglContext) {
		auto newProgram = this->program.reload(openglContext);

		if (newProgram.has_value()) {
			this->program = std::move(newProgram.value());

			return true;
		}
		else {
			return false;
		}
	}
	OpenglRenderer::OpenglRenderer(OpenglContext& openglContext_)
	    : openglContext(openglContext_),
	      program(openglContext) {
	}

	void OpenglRenderer::addUniform(std::string_view name, render::DataType uniformDataType, uint32_t count) {
	}
}
