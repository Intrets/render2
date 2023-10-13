#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <wrangled_gl/wrangled_gl.h>

#include "render/DataType.h"
#include "render/opengl/OpenglContext.h"
#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglVBO;

	struct VertexAttribute
	{
		DataType dataType{};

		enum class Divisor
		{
			zero,
			one
		} divisor{};

		GLuint getDivisor() const;

		int32_t offset{};
	};

	struct Descriptor
	{
		OpenglVAO& VAO;

		std::vector<VertexAttribute> attributes{};
		GLsizei stride{};

		Descriptor& add(
		    DataType dataType,
		    VertexAttribute::Divisor divisor
		);

		void finalize(OpenglVBO& VBO);
	};

	struct OpenglVAO
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		std::unordered_map<std::string, Descriptor> descriptors{};

		Descriptor& newDescriptor(std::string_view name);

		void bind();

		OpenglVAO(OpenglContext& openglContext);
		~OpenglVAO();
	};
}
