#include "render/opengl/OpenglPBO.h"

#include "render/opengl/OpenglContext.h"
#include "render/opengl/OpenglTexture.h"

namespace render::opengl
{
	void OpenglPBO::bindPack() {
		this->openglContext->bindPack(*this);
	}

	void OpenglPBO::bindUnpack() {
		this->openglContext->bindUnpack(*this);
	}

	void OpenglPBO::unbindPack() {
		this->openglContext->unbindPack();
	}

	void OpenglPBO::unbindUnpack() {
		this->openglContext->unbindUnpack();
	}

	void OpenglPBO::download(Opengl2DTexture& texture, integer_t level, TextureFormat::PixelFormat pixelFormat) {
		if (this->bufferMapped) {
			tassert(0);
			return;
		}

		auto dummy = texture.textureFormat;
		dummy.pixelFormat = pixelFormat;

		this->bindPack();
		glBufferData(
		    GL_PIXEL_PACK_BUFFER,
		    dummy.getByteSize(),
		    nullptr,
		    GL_STATIC_READ
		);
		this->downloadSize = dummy.getByteSize();

		texture.bind();
#ifndef WRANGLE_GLESv3
		glGetTexImage(
		    GL_TEXTURE_2D,
		    0,
		    dummy.getPixelDataFormat(),
		    dummy.getPixelDataType(),
		    nullptr
		);
#else
		tassert(0);
#endif
		this->unbindPack();
	}

	void OpenglPBO::upload(
	    integer_t level,
	    Opengl2DTexture& texture
	) {
		this->bindUnpack();
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		texture.bind();

		auto& textureFormat = this->uploadFormat.value();

		glTexImage2D(
		    GL_TEXTURE_2D,
		    static_cast<GLint>(level),
		    textureFormat.getInternalFormat(),
		    textureFormat.size.x,
		    textureFormat.size.y,
		    0,
		    textureFormat.getPixelDataFormat(),
		    textureFormat.getPixelDataType(),
		    nullptr
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFormat.getMagFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFormat.getMinFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureFormat.getWrappingX());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureFormat.getWrappingY());

		if (textureFormat.mipmapLevels > 1) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	void OpenglPBO::unmapPBO() {
		this->bindPack();
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		this->bufferMapped = false;
		this->unbindPack();
	}

	OpenglPBO::OpenglPBO(OpenglContext& openglContext_)
	    : openglContext(&openglContext_) {
		this->ID.qualifier = this->openglContext->getQualifier();

		glGenBuffers(1, &this->ID.data);
	}

	OpenglPBO::~OpenglPBO() {
		if (this->bufferMapped) {
			tassert(0);
			std::abort();
		}

		if (this->ID && this->ID.data != 0) {
			glDeleteBuffers(1, &this->ID.data);
		}
	}
}
