#include "render/opengl/OpenglFramebuffer.h"

#include "render/opengl/OpenglContext.h"
#include "render/opengl/OpenglTexture.h"

namespace render::opengl
{
	void OpenglFramebuffer::bind() {
		this->openglContext.bind(*this);
	}

	void OpenglFramebuffer::attach(Opengl2DTexture const& texture) {
		this->attach({ Attachment::Type::color0 }, texture, 0);
	}

	void OpenglFramebuffer::attach(Attachment attachment, Opengl2DTexture const& texture, GLint mipmap) {
		this->bind();

		glFramebufferTexture(GL_FRAMEBUFFER, attachment.get(), texture.ID.data, mipmap);
	}

	void OpenglFramebuffer::attach(Attachment attachment, Opengl2DArrayTexture const& texture, int32_t layer, GLint mipmap) {
		this->bind();

		glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment.get(), texture.ID.data, mipmap, layer);
	}

	void OpenglFramebuffer::clear(glm::vec4 color, bool depth) {
		this->bind();
		glClearColor(color.r, color.g, color.b, color.a);

		if (depth) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	OpenglFramebuffer::OpenglFramebuffer(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
		glGenFramebuffers(1, &this->ID.data);
	}

	OpenglFramebuffer::OpenglFramebuffer(OpenglContext& openglContext_, GLuint ID_)
	    : openglContext(openglContext_) {
		if (ID_ == 0) {
			this->ID.data = 0;
			this->ID.qualifier = this->openglContext.getScreenFramebufferQualifier();
		}
		else {
			this->ID.data = ID_;
			this->ID.qualifier = this->openglContext.getQualifier();
		}
	}

	OpenglFramebuffer::~OpenglFramebuffer() {
		glDeleteFramebuffers(1, &this->ID.data);
	}

	OpenglFramebuffer OpenglFramebuffer::screenTarget(OpenglContext& openglContext) {
		return OpenglFramebuffer(openglContext, 0);
	}

	GLenum OpenglFramebuffer::Attachment::get() const {
		te::enum_array<Type, GLenum> lookup{
			{ Type::color0, GL_COLOR_ATTACHMENT0 },
			{ Type::color1, GL_COLOR_ATTACHMENT1 },
			{ Type::color2, GL_COLOR_ATTACHMENT2 },
			{ Type::color3, GL_COLOR_ATTACHMENT3 },
			{ Type::color4, GL_COLOR_ATTACHMENT4 },
			{ Type::color5, GL_COLOR_ATTACHMENT5 },
			{ Type::color6, GL_COLOR_ATTACHMENT6 },
			{ Type::color7, GL_COLOR_ATTACHMENT7 },
			{ Type::color8, GL_COLOR_ATTACHMENT8 },
			{ Type::color9, GL_COLOR_ATTACHMENT9 },
			{ Type::depth, GL_DEPTH_ATTACHMENT },
			{ Type::depthStencil, GL_DEPTH_STENCIL_ATTACHMENT },
			{ Type::stencil, GL_STENCIL_ATTACHMENT },
		};

		return lookup[this->type];
	}
}
