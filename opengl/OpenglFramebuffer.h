#pragma once

#include <wglm/glm.hpp>

#include <wrangled_gl/wrangled_gl.h>

#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglContext;
	struct Opengl2DTexture;
	struct Opengl2DArrayTexture;

	struct OpenglFramebuffer
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		struct Attachment
		{
			enum class Type
			{
				color0,
				color1,
				color2,
				color3,
				color4,
				color5,
				color6,
				color7,
				color8,
				color9,
				depth,
				stencil,
				depthStencil,
				MAX
			} type{};

			GLenum get() const;
		};

		void bind();
		void attach(Opengl2DTexture const& texture);
		void attach(Attachment attachment, Opengl2DTexture const& texture, GLint mipmap);
		void attach(Attachment attachment, Opengl2DArrayTexture const& texture, int32_t layer, GLint mipmap);

		void clear(glm::vec4 color, bool depth);

		OpenglFramebuffer() = delete;
		OpenglFramebuffer(OpenglContext& openglContext);
		explicit OpenglFramebuffer(OpenglContext& openglContext, GLuint ID);

		NO_COPY(OpenglFramebuffer);
		DEFAULT_MOVE(OpenglFramebuffer);

		~OpenglFramebuffer();

		static OpenglFramebuffer screenTarget(OpenglContext& openglContext);
	};
}