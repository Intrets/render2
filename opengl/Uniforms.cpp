#include "render/opengl/Uniforms.h"

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	void OpenglSampler2D::initialize(te::cstring_view name, Program& program_) {
		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler2D::set(Opengl2DTexture const& texture) {
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}

	void OpenglSampler3D::initialize(te::cstring_view name, Program& program_) {
		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler3D::set(Opengl2DArrayTexture const& texture) {
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}

	void OpenglSamplerBufferTexture::initialize(te::cstring_view name, Program& program_) {
		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();
		glUniform1i(this->location, this->unit);
	}

	void OpenglSamplerBufferTexture::set(OpenglBufferTexture const& texture) {
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}
}
