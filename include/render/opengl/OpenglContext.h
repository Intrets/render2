#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <tepp/enum_array.h>

#include "render/opengl/BufferTarget.h"
#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglVAO;
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

		te::enum_array<BufferTarget::Type, Qualified<GLuint>> boundBuffers{};

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

		void reset();

		int64_t getQualifier();

		template<class... Args>
		void logInfo(std::string_view str, Args&&... args) {
		}

		template<class... Args>
		void logWarning(std::string_view str, Args&&... args) {
		}

		template<class... Args>
		void logError(std::string_view str, Args&&... args) {
		}
	};
}