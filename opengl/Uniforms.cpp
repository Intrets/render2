#include "render/opengl/Uniforms.h"

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	void OpenglSampler2D::initialize(te::cstring_view name, Program& program) {
		this->program = &program;
		this->location = glGetUniformLocation(program.ID.data, name.getData());
		this->unit = program.getNextSampler();

		this->program->use();
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler2D::set(Opengl2DTexture& texture) {
		assert(this->program);

		this->program->openglContext.bind(texture, this->unit);
	}

	void OpenglSampler3D::initialize(te::cstring_view name, Program& program) {
		this->program = &program;
		this->location = glGetUniformLocation(program.ID.data, name.getData());
		this->unit = program.getNextSampler();

		this->program->use();
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler3D::set(Opengl2DArrayTexture& texture) {
		assert(this->program);

		this->program->openglContext.bind(texture, this->unit);
	}
}
