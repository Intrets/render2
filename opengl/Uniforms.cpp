#include "render/opengl/Uniforms.h"

#include "render/opengl/OpenglContext.h"

#include <tepp/safety_cast.h>

namespace render::opengl
{
	void OpenglSampler2D::initialize(te::cstring_view name, Program& program_) {
		this->initialize(name, 1, program_);
	}

	void OpenglSampler2D::initialize(te::cstring_view name, integer_t count, Program& program_) {
		auto refresh = this->program != nullptr;

		this->program = &program_;
		this->program->use();
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());

		if (!refresh) {
			tassert(this->units.empty());
			for (int i = 0; i < count; i++) {
				this->units.push_back(this->program->getNextSampler());
			}
		}

		tassert(isize(this->units) == count);

		this->program->openglContext.tallyUniformBytesTransferred(std::span(this->units).size_bytes());
		glUniform1iv(this->location, static_cast<GLsizei>(count), units.data());
	}

	void OpenglSampler2D::set(Opengl2DTexture const& texture) {
		tassert(this->program);
		tassert(!this->units.empty());

		this->program->use();
		this->program->openglContext.bind(texture, this->units.front());
	}

	void OpenglSampler2D::set(Opengl2DTexture const& texture, integer_t index) {
		if (!(0 <= index && index < isize(this->units))) {
			tassert(0);
			return;
		}

		tassert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->units[index]);
	}

	void OpenglSampler2D::set(Qualified<GLuint> ID, integer_t index) {
		if (!(0 <= index && index < isize(this->units))) {
			tassert(0);
			return;
		}

		tassert(this->program);

		this->program->use();
		this->program->openglContext.bind(ID, TextureTarget::Type::TEXTURE_2D, this->units[index]);
	}

	void OpenglSampler3D::initialize(te::cstring_view name, Program& program_) {
		auto refresh = this->program != nullptr;

		this->program = &program_;
		this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		this->unit = this->program->getNextSampler();

		this->program->use();

		if (!refresh) {
			this->unit = this->program->getNextSampler();
		}

		this->program->openglContext.tallyUniformBytesTransferred(sizeof(this->unit));
		glUniform1i(this->location, this->unit);
	}

	void OpenglSampler3D::set(Opengl2DArrayTexture const& texture) {
		tassert(this->program);

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
		tassert(this->program);

		this->program->use();
		this->program->openglContext.bind(texture, this->unit);
	}
}
