#pragma once

#include <format>
#include <iostream>

#include <wrangled_gl/wrangled_gl.h>

#include <vec/ivec4.h>

#include <tepp/enum_array.h>

#include "render/opengl/BufferTarget.h"
#include "render/opengl/Qualifier.h"
#include "render/opengl/TextureTarget.h"

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

	struct Configuration
	{
		Blend blend = Blend::UNSET;
		BlendFunc blendFunc = BlendFunc::UNSET;
		DepthTest depthTest = DepthTest::UNSET;
		DepthFunc depthFunc = DepthFunc::UNSET;
		PolygonMode polygonMode = PolygonMode::UNSET;
		float pointSize = 0.0f;
		DepthMask depthMask = DepthMask::UNSET;
	};

	struct OpenglContext
	{
		int64_t qualifierCounter{};

		Qualified<GLuint> boundVAO{};
		Qualified<GLuint> usedProgram{};
		Qualified<GLuint> boundFramebuffer{};

		te::enum_array<BufferTarget::Type, Qualified<GLuint>> boundBuffers{};
		te::enum_array<TextureTarget::Type, Qualified<GLuint>> boundTextures{};
		std::vector<Qualified<GLuint>> boundSamplerUnits{};
		int32_t activeUnit = 0;

		vec::ivec4 viewport{};

		Configuration configuration{};

		void setBlend(Blend b);
		void setBlendFunc(BlendFunc func);
		void setDepthTest(DepthTest test);
		void setDepthFunc(DepthFunc func);
		void setPolygonMode(PolygonMode mode);
		void setDepthMask(DepthMask b);

		void bind(OpenglVAO& openglVAO);
		void bind(OpenglVBO& openglVBO, BufferTarget target = {});
		void use(Program& program);
		void bind(Opengl2DTexture const& texture);
		void bind(Opengl2DTexture const& texture, int32_t unit);
		void bind(Opengl2DArrayTexture const& texture);
		void bind(Opengl2DArrayTexture const& texture, int32_t unit);
		void bind(OpenglBufferTexture const& texture);
		void bind(OpenglBufferTexture const& texture, int32_t unit);
		void bind(OpenglFramebuffer& framebuffer);

		void setViewport(vec::ivec4 viewport);

		void reset();

		int64_t getQualifier();
		int64_t getScreenFramebufferQualifier() const;

		OpenglContext();
		~OpenglContext() = default;

		template<class... Args>
		void logInfo(std::string_view str, Args&&... args) {
			std::cout << "Info: " << std::vformat(str, std::make_format_args(args...));
		}

		template<class... Args>
		void logWarning(std::string_view str, Args&&... args) {
			std::cout << "Warning: " << std::vformat(str, std::make_format_args(args...));
		}

		template<class... Args>
		void logError(std::string_view str, Args&&... args) {
			std::cout << "Error: " << std::vformat(str, std::make_format_args(args...));
		}
	};
}