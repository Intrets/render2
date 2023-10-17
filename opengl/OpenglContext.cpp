#include "render/opengl/OpenglContext.h"

#include "render/opengl/OpenglFramebuffer.h"
#include "render/opengl/OpenglTexture.h"
#include "render/opengl/OpenglVAO.h"
#include "render/opengl/OpenglVBO.h"
#include "render/opengl/Program.h"

namespace render::opengl
{
	void OpenglContext::setBlend(Blend b) {
		if (this->configuration.blend != b) {
			this->configuration.blend = b;
			switch (b) {
				case Blend::ENABLED:
					glEnable(GL_BLEND);
					break;
				case Blend::DISABLED:
					glDisable(GL_BLEND);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::setBlendFunc(BlendFunc func) {
		if (this->configuration.blendFunc != func) {
			this->configuration.blendFunc = func;
			switch (func) {
				case BlendFunc::SRC_ALPHA__ONE_MINUS_SRC_ALPHA:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case BlendFunc::SRC_ONE__ONE_MINUS_SRC_ALPHA:
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case BlendFunc::SEPERATE____GL_SRC_ALPHA__GL_ONE_MINUS_SRC_ALPHA___GL_ONE_MINUS_DST_ALPHA__GL_ONE:
					glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
					break;
				case BlendFunc::ONE__ZERO:
					glBlendFunc(GL_ONE, GL_ZERO);
					break;
				case BlendFunc::ZERO__ONE:
					glBlendFunc(GL_ZERO, GL_ONE);
					break;
				case BlendFunc::ONE__ONE:
					glBlendFunc(GL_ONE, GL_ONE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::setDepthTest(DepthTest test) {
		if (this->configuration.depthTest != test) {
			this->configuration.depthTest = test;
			switch (test) {
				case DepthTest::ENABLED:
					glEnable(GL_DEPTH_TEST);
					break;
				case DepthTest::DISABLED:
					glDisable(GL_DEPTH_TEST);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::setDepthFunc(DepthFunc func) {
		if (this->configuration.depthFunc != func) {
			this->configuration.depthFunc = func;
			switch (func) {
				case DepthFunc::LESS:
					glDepthFunc(GL_LESS);
					break;
				case DepthFunc::LEQUAL:
					glDepthFunc(GL_LEQUAL);
					break;
				case DepthFunc::ALWAYS:
					glDepthFunc(GL_ALWAYS);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::setPolygonMode(PolygonMode mode) {
		if (this->configuration.polygonMode != mode) {
			if (this->configuration.polygonMode == PolygonMode::POINT) {
				glDisable(GL_PROGRAM_POINT_SIZE);
			}

			this->configuration.polygonMode = mode;
			switch (mode) {
				case PolygonMode::FILL:
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					break;
				case PolygonMode::LINE:
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					break;
				case PolygonMode::POINT:
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
					glEnable(GL_PROGRAM_POINT_SIZE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::setDepthMask(DepthMask b) {
		if (this->configuration.depthMask != b) {
			this->configuration.depthMask = b;
			switch (b) {
				case DepthMask::FALSE:
					glDepthMask(GL_FALSE);
					break;
				case DepthMask::TRUE:
					glDepthMask(GL_TRUE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void OpenglContext::bind(OpenglVAO& openglVAO) {
		if (this->boundVAO != openglVAO.ID) {
			glBindVertexArray(openglVAO.ID.data);
			this->boundVAO = openglVAO.ID;
		}
	}

	void OpenglContext::bind(OpenglVBO& openglVBO, BufferTarget bufferTarget) {
		if (this->boundBuffers[bufferTarget] != openglVBO.ID) {
			glBindBuffer(bufferTarget.get(), openglVBO.ID.data);
			this->boundBuffers[bufferTarget] = openglVBO.ID;
		}
	}

	void OpenglContext::use(Program& program) {
		if (this->usedProgram != program.ID) {
			glUseProgram(program.ID.data);
			this->usedProgram = program.ID;
		}
	}

	void OpenglContext::bind(Opengl2DTexture& opengl2DTexture) {
		if (this->boundTextures[TextureTarget::Type::TEXTURE_2D] != opengl2DTexture.ID) {
			glBindTexture(TextureTarget(TextureTarget::Type::TEXTURE_2D).get(), opengl2DTexture.ID.data);
			this->boundSamplerUnits[this->activeUnit] = opengl2DTexture.ID;
			this->boundTextures[TextureTarget::Type::TEXTURE_2D] = opengl2DTexture.ID;
		}
	}

	void OpenglContext::bind(Opengl2DTexture& texture, int32_t unit) {
		if (unit < 0 || unit >= this->boundSamplerUnits.size()) {
			return;
		}

		if (this->boundSamplerUnits[unit] != texture.ID) {
			glActiveTexture(GL_TEXTURE0 + unit);
			this->activeUnit = unit;
			this->bind(texture);
		}
	}

	void OpenglContext::bind(Opengl2DArrayTexture& texture) {
		if (this->boundTextures[TextureTarget::Type::TEXTURE_2D_ARRAY] != texture.ID) {
			glBindTexture(TextureTarget(TextureTarget::Type::TEXTURE_2D_ARRAY).get(), texture.ID.data);
			this->boundSamplerUnits[this->activeUnit] = texture.ID;
			this->boundTextures[TextureTarget::Type::TEXTURE_2D_ARRAY] = texture.ID;
		}
	}

	void OpenglContext::bind(Opengl2DArrayTexture& texture, int32_t unit) {
		if (unit < 0 || unit >= this->boundSamplerUnits.size()) {
			return;
		}

		if (this->boundSamplerUnits[unit] != texture.ID) {
			glActiveTexture(GL_TEXTURE0 + unit);
			this->activeUnit = unit;
			this->bind(texture);
		}
	}

	void OpenglContext::bind(OpenglFramebuffer& framebuffer) {
		if (this->boundFramebuffer != framebuffer.ID) {
			this->boundFramebuffer = framebuffer.ID;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.ID.data);
		}
	}

	void OpenglContext::setViewport(vec::ivec4 viewport_) {
		if (this->viewport != viewport_) {
			this->viewport = viewport_;
			glViewport(
			    this->viewport[0],
			    this->viewport[1],
			    this->viewport[2],
			    this->viewport[3]
			);
		}
	}

	void OpenglContext::reset() {
		this->usedProgram = {};
		this->boundVAO = {};
		this->boundBuffers = {};
	}

	int64_t OpenglContext::getQualifier() {
		return this->qualifierCounter++;
	}

	int64_t OpenglContext::getScreenFramebufferQualifier() const {
		return -1;
	}

	OpenglContext::OpenglContext() {
		int maximumTextureUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maximumTextureUnits);

		this->boundSamplerUnits.resize(maximumTextureUnits);
	}
}