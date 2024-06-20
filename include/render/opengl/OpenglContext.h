#pragma once

#include <format>
#include <iostream>

#include <wrangled_gl/wrangled_gl.h>

#include <tepp/cstring_view.h>
#include <tepp/enum_array.h>

#include "render/opengl/BufferTarget.h"
#include "render/opengl/ProgramRegistry.h"
#include "render/opengl/Qualifier.h"
#include "render/opengl/TextureTarget.h"

#include <wglm/vec4.hpp>

namespace render::opengl
{
	struct OpenglVAO;
	struct Opengl2DTexture;
	struct OpenglBufferTexture;
	struct Opengl2DArrayTexture;
	struct OpenglFramebuffer;
	struct OpenglVBO;
	struct Program;

	enum class DepthMask
	{
		UNSET,
		FALSE,
		TRUE,
		MAX,
	};

	enum class Blend
	{
		UNSET,
		ENABLED,
		DISABLED,
		MAX,
	};

	enum class BlendFunc
	{
		UNSET,
		SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
		SRC_ONE__ONE_MINUS_SRC_ALPHA,
		SEPERATE____GL_SRC_ALPHA__GL_ONE_MINUS_SRC_ALPHA___GL_ONE_MINUS_DST_ALPHA__GL_ONE,
		ONE__ZERO,
		ZERO__ONE,
		ONE__ONE,
		MAX,
	};

	enum class DepthTest
	{
		UNSET,
		ENABLED,
		DISABLED,
		MAX,
	};

	enum class DepthFunc
	{
		UNSET,
		LESS,
		LEQUAL,
		ALWAYS,
		MAX,
	};

	enum class PolygonMode
	{
		UNSET,
		FILL,
		LINE,
		POINT,
		MAX,
	};

	enum class SRGBMode
	{
		UNSET,
		ON,
		OFF,
		MAX
	};

	struct Configuration
	{
		Blend blend = Blend::UNSET;
		BlendFunc blendFunc = BlendFunc::UNSET;
		DepthTest depthTest = DepthTest::UNSET;
		DepthFunc depthFunc = DepthFunc::UNSET;
		float pointSize = 0.0f;
		DepthMask depthMask = DepthMask::UNSET;
		SRGBMode srgbMode = SRGBMode::UNSET;

		static Configuration getDefault();
	};

	struct OpenglContext
	{
		qualifier_t qualifierCounter{};

		Qualified<GLuint> boundVAO{};
		Qualified<GLuint> usedProgram{};
		Qualified<GLuint> boundFramebuffer{};

		te::enum_array<BufferTarget::Type, Qualified<GLuint>> boundBuffers{};
		te::enum_array<TextureTarget::Type, Qualified<GLuint>> boundTextures{};
		struct SamplerUnitInfo
		{
			Qualified<GLuint> texture{};
			TextureTarget type{};
		};
		std::vector<SamplerUnitInfo> boundSamplerUnits{};
		integer_t activeUnit = 0;

		glm::ivec4 viewport{};

		std::ostream* out = &std::cout;

		Configuration configuration{};

		ProgramRegistry programRegistry{};

		struct BytesTransferredInfo
		{
			integer_t bufferDataCalls{};
			integer_t buffers{};
			integer_t setUniformCalls{};
			integer_t uniforms{};
			integer_t programSwitches{};
			integer_t VAOSwitches{};
			integer_t drawCalls{};
		};
		BytesTransferredInfo bytesTransferredThisFrame{};
		BytesTransferredInfo bytesTransferredLastFrame{};

		enum class ShaderVersion
		{
			version_330,
			version_320_es,
			MAX
		} shaderVersion = ShaderVersion::version_330;

		te::enum_array<ShaderVersion, te::cstring_view> shaderPrefixes{
			{ ShaderVersion::version_330, "#version 330\n" },
			{ ShaderVersion::version_320_es, "#version 320 es\nprecision mediump float;\n" },
		};

		te::cstring_view getShaderPrefix() const;

		std::string_view trimShaderPrefix(std::string_view shaderSource);
		void setShaderSource(GLint ID, std::string_view source);

		void setConfiguration(Configuration const& configuration);

		void setBlend(Blend b);
		void setBlendFunc(BlendFunc func);
		void setDepthTest(DepthTest test);
		void setDepthFunc(DepthFunc func);
		void setDepthMask(DepthMask b);
		void setSRGBMode(SRGBMode mode);

		void bind(OpenglVAO& openglVAO);
		void bind(OpenglVBO& openglVBO, BufferTarget target = {});
		void bindTextureUnit(integer_t unit);
		void use(Program& program);
		void bind(Qualified<GLuint> ID, TextureTarget target, int32_t unit);
		void bind(Opengl2DTexture const& texture);
		void bind(Opengl2DTexture const& texture, int32_t unit);
		void bind(Opengl2DArrayTexture const& texture);
		void bind(Opengl2DArrayTexture const& texture, int32_t unit);
		void bind(OpenglBufferTexture const& texture);
		void bind(OpenglBufferTexture const& texture, int32_t unit);
		void bind(OpenglFramebuffer& framebuffer);

		void setViewport(glm::ivec4 viewport);

		void reset();

		void registerProgram(Program& program);
		void unRegisterProgram(Program& program);
		void moveProgram(Program& from, Program& to);

		qualifier_t getQualifier();
		qualifier_t getScreenFramebufferQualifier() const;

		void tallySwitchProgram();
		void tallySwitchVAO();
		void tallyDrawCall();

		void tallyBytesTransferred(integer_t bytes);
		void tallyUniformBytesTransferred(integer_t bytes);
		void cycle();

		OpenglContext(ShaderVersion shaderVersion_);
		~OpenglContext();

		template<class... Args>
		void logInfo(std::string_view str, Args&&... args) {
			*this->out << "Info: " << std::vformat(str, std::make_format_args(args...));
		}

		template<class... Args>
		void logWarning(std::string_view str, Args&&... args) {
			*this->out << "Warning: " << std::vformat(str, std::make_format_args(args...));
		}

		template<class... Args>
		void logError(std::string_view str, Args&&... args) {
			*this->out << "Error: " << std::vformat(str, std::make_format_args(args...));
		}
	};
}