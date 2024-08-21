#include "render/opengl/OpenglPBO.h"

#include "render/opengl/OpenglContext.h"
#include "render/opengl/OpenglTexture.h"

namespace render::opengl
{
	void OpenglPBO::bind() {
		this->openglContext->bind(*this);
	}

	void OpenglPBO::download(Opengl2DTexture& texture, integer_t level, TextureFormat::PixelFormat pixelFormat) {
		if (this->bufferMapped) {
			assert(0);
			return;
		}

		auto dummy = texture.textureFormat;
		dummy.pixelFormat = pixelFormat;

		this->bind();
		glBufferData(
		    GL_PIXEL_PACK_BUFFER,
		    dummy.getByteSize(),
		    nullptr,
		    GL_STATIC_READ
		);
		this->downloadSize = dummy.getByteSize();

		texture.bind();
		glGetTexImage(
		    GL_TEXTURE_2D,
		    0,
		    dummy.getPixelDataFormat(),
		    dummy.getPixelDataType(),
		    nullptr
		);
	}

	void OpenglPBO::unmapDownload() {
		this->bind();
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		this->bufferMapped = false;
	}

	OpenglPBO::OpenglPBO(OpenglContext& openglContext_)
	    : openglContext(&openglContext_) {
		this->ID.qualifier = this->openglContext->getQualifier();

		glGenBuffers(1, &this->ID.data);
	}

	OpenglPBO::~OpenglPBO() {
		if (this->bufferMapped) {
			assert(0);
			std::abort();
		}

		if (this->ID && this->ID.data != 0) {
			glDeleteBuffers(1, &this->ID.data);
		}
	}
}
