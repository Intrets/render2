#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglContext;
	struct OpenglVBO;

	struct OpenglBufferTexture
	{
		enum class InternalFormat
		{
			R32F,
			RGBA32F,
			RGB32F,
			RGB32UI,
			RGB32I,
			MAX
		} internalFormat{};

		int32_t getInternalFormatByteSize() const;
		GLenum getInternalFormat() const;

		OpenglContext& openglContext;
		Qualified<GLuint> ID{};
		Qualified<GLuint> boundVBO{};

		void bind(OpenglVBO& VBO);

		OpenglBufferTexture(OpenglContext& openglContext, InternalFormat internalFormat);
		~OpenglBufferTexture();
	};
}