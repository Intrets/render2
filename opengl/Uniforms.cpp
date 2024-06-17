#include "render/opengl/Uniforms.h"

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	void OpenglSampler2D::initialize(te::cstring_view name, Program& program_) {
		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();

		this->program->openglContext.tallyUniformBytesTransferred(sizeof(this->unit));
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler2D::initialize(te::cstring_view name, integer_t count_, Program& program_) {
		assert(count_ > 0);

		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->count = count_;

		this->program->use();
		std::vector<GLint> units{};
		for (int i = 0; i < count; i++) {
			units.push_back(this->program->getNextSampler());
		}

		this->unit = units.front();
		glUniform1iv(this->location, static_cast<GLsizei>(count), units.data());
	}

	void OpenglSampler2D::set(Opengl2DTexture const& texture) {
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}

	void OpenglSampler2D::set(Opengl2DTexture const& texture, integer_t index) {
		assert(index < this->count);
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit + static_cast<GLint>(index));
	}

	void OpenglSampler3D::initialize(te::cstring_view name, Program& program_) {
		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();

		this->program->openglContext.tallyUniformBytesTransferred(sizeof(this->unit));
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

		this->program->openglContext.tallyUniformBytesTransferred(sizeof(this->unit));
		glUniform1i(this->location, this->unit);
	}

	void OpenglSamplerBufferTexture::set(OpenglBufferTexture const& texture) {
		assert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}
}
