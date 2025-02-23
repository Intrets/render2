#include "render/opengl/OpenglContext.h"

#include "render/opengl/OpenglBufferTexture.h"
#include "render/opengl/OpenglFramebuffer.h"
#include "render/opengl/OpenglPBO.h"
#include "render/opengl/OpenglTexture.h"
#include "render/opengl/OpenglVAO.h"
#include "render/opengl/OpenglVBO.h"
#include "render/opengl/Program.h"

#include <tepp/safety_cast.h>

#include <ranges>
#include <algorithm>

namespace render::opengl
{
	te::cstring_view OpenglContext::getShaderPrefix() const {
		return shaderPrefixes[this->shaderVersion];
	}

	std::string_view OpenglContext::trimShaderPrefix(std::string_view shaderSource) {
		auto i = shaderSource.find('#');
		shaderSource = shaderSource.substr(i);

		i = shaderSource.find('\n');
		shaderSource = shaderSource.substr(i + 1);

		i = shaderSource.find("precision");
		shaderSource = shaderSource.substr(i);

		i = shaderSource.find(';');
		shaderSource = shaderSource.substr(i + 1);

		return shaderSource;
	}

	void OpenglContext::setShaderSource(GLint ID, std::string_view source) {
		auto trimmedSource = this->trimShaderPrefix(source);

		std::array<std::string_view, 2> shaderSource = {
			this->getShaderPrefix().getData(),
			trimmedSource
		};

		std::array<char const*, 2> pointers{
			shaderSource[0].data(),
			shaderSource[1].data(),
		};

		std::array<GLint, 2> sizes{
			te::safety_cast<GLint>(shaderSource[0].size()),
			te::safety_cast<GLint>(shaderSource[1].size()),
		};

		glShaderSource(ID, static_cast<GLsizei>(shaderSource.size()), pointers.data(), sizes.data());
	}

	void OpenglContext::setConfiguration(Configuration const& configuration_) {
		setBlend(configuration_.blend);
		setBlendFunc(configuration_.blendFunc);
		setBlendEquation(configuration_.blendEquation);
		setDepthTest(configuration_.depthTest);
		setDepthFunc(configuration_.depthFunc);
		setDepthMask(configuration_.depthMask);
		setSRGBMode(configuration_.srgbMode);

		this->configuration = configuration_;
	}

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
					tassert(0);
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
				case BlendFunc::DST_COLOR__ZERO:
					glBlendFunc(GL_DST_COLOR, GL_ZERO);
					break;
				default:
					tassert(0);
					break;
			}
		}
	}

	void OpenglContext::setBlendEquation(BlendEquation equation) {
		if (this->configuration.blendEquation != equation) {
			this->configuration.blendEquation = equation;
			switch (equation) {
				case BlendEquation::FUNC_MAX:
					glBlendEquation(GL_MAX);
					break;
				case BlendEquation::FUNC_MIN:
					glBlendEquation(GL_MIN);
					break;
				case BlendEquation::FUNC_ADD:
					glBlendEquation(GL_FUNC_ADD);
					break;
				case BlendEquation::FUNC_REVERSE_SUBTRACT:
					glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
					break;
				case BlendEquation::FUNC_SUBTRACT:
					glBlendEquation(GL_FUNC_SUBTRACT);
					break;
				default:
					tassert(0);
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
					tassert(0);
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
					tassert(0);
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
					tassert(0);
					break;
			}
		}
	}

	void OpenglContext::setSRGBMode(SRGBMode mode) {
#ifndef WRANGLE_GLESv3
		if (this->configuration.srgbMode != mode) {
			this->configuration.srgbMode = mode;
			switch (mode) {
				case SRGBMode::ON:
					glEnable(GL_FRAMEBUFFER_SRGB);
					break;
				case SRGBMode::OFF:
					glDisable(GL_FRAMEBUFFER_SRGB);
					break;
				default:
					tassert(0);
					break;
			}
		}
#endif
	}

	void OpenglContext::bind(OpenglVAO& openglVAO) {
		if (this->boundVAO != openglVAO.ID) {
			this->tallySwitchVAO();
			glBindVertexArray(openglVAO.ID.data);
			this->boundVAO = openglVAO.ID;
		}
	}

	void OpenglContext::bindPack(OpenglPBO& openglPBO) {
		if (this->boundPackPBO != openglPBO.ID) {
			glBindBuffer(GL_PIXEL_PACK_BUFFER, openglPBO.ID.data);
			this->boundPackPBO = openglPBO.ID;
		}
	}

	void OpenglContext::bindUnpack(OpenglPBO& openglPBO) {
		if (this->boundUnpackPBO != openglPBO.ID) {
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, openglPBO.ID.data);
			this->boundUnpackPBO = openglPBO.ID;
		}
	}

	void OpenglContext::unbindPack() {
		if (this->boundPackPBO) {
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			this->boundPackPBO = {};
		}
	}

	void OpenglContext::unbindUnpack() {
		if (this->boundUnpackPBO) {
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			this->boundUnpackPBO = {};
		}
	}

	void OpenglContext::bind(OpenglVBO& openglVBO, BufferTarget bufferTarget) {
		if (this->boundBuffers[bufferTarget] != openglVBO.ID) {
			glBindBuffer(bufferTarget.get(), openglVBO.ID.data);
			this->boundBuffers[bufferTarget] = openglVBO.ID;
		}
	}

	void OpenglContext::bindTextureUnit(integer_t unit) {
		if (this->activeUnit != unit) {
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));
			this->activeUnit = unit;
		}
	}

	void OpenglContext::use(Program& program) {
		if (this->usedProgram != program.ID) {
			this->tallySwitchProgram();
			glUseProgram(program.ID.data);
			this->usedProgram = program.ID;
		}
	}

	void OpenglContext::bind(Qualified<GLuint> ID, TextureTarget target, int32_t unit) {
		auto& samplerUnitInfo = this->boundSamplerUnits[unit];

		if (!samplerUnitInfo.type.unbound() && samplerUnitInfo.type != target) {
			this->bindTextureUnit(unit);
			glBindTexture(samplerUnitInfo.type.get(), 0);
			samplerUnitInfo.texture = {};
		}

		if (samplerUnitInfo.texture != ID) {
			this->bindTextureUnit(unit);
			glBindTexture(target.get(), ID.data);
		}

		samplerUnitInfo.texture = ID;
		samplerUnitInfo.type = target;
	}

	void OpenglContext::bind(Opengl2DTexture const& opengl2DTexture) {
		this->bind(opengl2DTexture, 0);
	}

	void OpenglContext::bind(Opengl2DTexture const& texture, int32_t unit) {
		this->bind(texture.ID, TextureTarget::Type::TEXTURE_2D, unit);
	}

	void OpenglContext::bind(Opengl2DArrayTexture const& texture) {
		this->bind(texture, 0);
	}

	void OpenglContext::bind(Opengl2DArrayTexture const& texture, int32_t unit) {
		this->bind(texture.ID, TextureTarget::Type::TEXTURE_2D_ARRAY, unit);
	}

	void OpenglContext::bind(OpenglBufferTexture const& texture) {
		this->bind(texture, 0);
	}

	void OpenglContext::bind(OpenglBufferTexture const& texture, int32_t unit) {
		this->bind(texture.ID, TextureTarget::Type::TEXTURE_BUFFER, unit);
	}

	void OpenglContext::bind(OpenglFramebuffer& framebuffer) {
		if (this->boundFramebuffer != framebuffer.ID) {
			this->boundFramebuffer = framebuffer.ID;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.ID.data);
		}
	}

	void OpenglContext::setViewport(glm::ivec4 viewport_) {
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
		if (this->boundPackPBO) {
			glBindFramebuffer(GL_PIXEL_PACK_BUFFER, 0);
		}
		this->boundPackPBO = {};
		if (this->boundUnpackPBO) {
			glBindFramebuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		this->boundUnpackPBO = {};
		this->boundFramebuffer = {};
		this->boundBuffers.fill({});
		this->configuration = {};
		this->boundTextures.fill({});

		{
			integer_t unit = 0;
			for (auto& samplerUnitInfo : this->boundSamplerUnits) {
				if (samplerUnitInfo.texture.data != 0) {
					this->bindTextureUnit(unit);
					glBindTexture(samplerUnitInfo.type.get(), 0);
				}

				samplerUnitInfo = {};
			}

			unit++;
		}

		this->activeUnit = 0;
		glActiveTexture(GL_TEXTURE0);

		viewport = {};
	}

	void OpenglContext::registerProgram(Program& program) {
		this->programRegistry.registerProgram(program);
	}

	void OpenglContext::registerProgram(Program& program, ProgramDescription description) {
		this->programRegistry.registerProgram(program, description);
	}

	std::optional<ProgramDescription> OpenglContext::unRegisterProgram(Program& program) {
		return this->programRegistry.unRegisterProgram(program);
	}

	qualifier_t OpenglContext::getQualifier() {
		return this->qualifierCounter++;
	}

	qualifier_t OpenglContext::getScreenFramebufferQualifier() const {
		return -1;
	}

	void OpenglContext::tallySwitchProgram() {
		this->bytesTransferredThisFrame.programSwitches++;
	}

	void OpenglContext::tallySwitchVAO() {
		this->bytesTransferredThisFrame.VAOSwitches++;
	}

	void OpenglContext::tallyDrawCall() {
		this->bytesTransferredThisFrame.drawCalls++;
	}

	void OpenglContext::tallyBytesTransferred(integer_t bytes) {
		this->bytesTransferredThisFrame.bufferDataCalls++;
		this->bytesTransferredThisFrame.buffers += bytes;
	}

	void OpenglContext::tallyUniformBytesTransferred(integer_t bytes) {
		this->bytesTransferredThisFrame.setUniformCalls++;
		this->bytesTransferredThisFrame.uniforms += bytes;
	}

	void OpenglContext::cycle() {
		this->bytesTransferredLastFrame = this->bytesTransferredThisFrame;
		this->bytesTransferredThisFrame = {};
	}

	OpenglContext::OpenglContext(ShaderVersion shaderVersion_)
	    : shaderVersion(shaderVersion_) {
		int maximumTextureUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maximumTextureUnits);

		this->boundSamplerUnits.resize(maximumTextureUnits);
	}

	OpenglContext::~OpenglContext() {
	}

	Configuration Configuration::getDefault() {
		return Configuration{
			.blend = Blend::ENABLED,
			.blendFunc = BlendFunc::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			.blendEquation = BlendEquation::FUNC_ADD,
			.depthTest = DepthTest::DISABLED,
			.depthFunc = DepthFunc::LESS,
			.pointSize = 1.0f,
			.depthMask = DepthMask::TRUE,
			.srgbMode = SRGBMode::ON,
		};
	}
}