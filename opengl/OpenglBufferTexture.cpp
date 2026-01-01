#include "render/opengl/OpenglBufferTexture.h"

#include "render/opengl/OpenglContext.h"
#include "render/opengl/OpenglVBO.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	int32_t OpenglBufferTexture::getInternalFormatByteSize() const {
		constexpr te::enum_array<InternalFormat, int32_t> lookup{
			{ InternalFormat::R32F, 4 },
			{ InternalFormat::RGBA32F, 16 },
			{ InternalFormat::RGB32F, 12 },
			{ InternalFormat::RGB32UI, 12 },
			{ InternalFormat::RGB32I, 12 },
		};

		return lookup[this->internalFormat];
	}

	GLenum OpenglBufferTexture::getInternalFormat() const {
		constexpr te::enum_array<InternalFormat, GLenum> lookup{
			{ InternalFormat::R32F, GL_R32F },
			{ InternalFormat::RGBA32F, GL_RGBA32F },
			{ InternalFormat::RGB32F, GL_RGB32F },
			{ InternalFormat::RGB32UI, GL_RGB32UI },
			{ InternalFormat::RGB32I, GL_RGB32I },
		};

		return lookup[this->internalFormat];
	}

	void OpenglBufferTexture::bind(OpenglVBO& VBO) {
		if (this->boundVBO == VBO.ID) {
			return;
		}

		if (this->getInternalFormatByteSize() != VBO.bufferSizeInformation.elementByteSize) {
			this->openglContext.logError("Mis-matched element size when binding VBO to BufferTexture.\n");
			return;
		}

		this->openglContext.bind(*this);
		glTexBuffer(GL_TEXTURE_BUFFER, this->getInternalFormat(), VBO.ID.data);
		this->boundVBO = VBO.ID;
	}

	OpenglBufferTexture::OpenglBufferTexture(OpenglContext& openglContext_, InternalFormat internalFormat_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
		glGenTextures(1, &this->ID.data);
		this->internalFormat = internalFormat_;
	}

	OpenglBufferTexture::~OpenglBufferTexture() {
		glDeleteTextures(1, &this->ID.data);
	}
}
