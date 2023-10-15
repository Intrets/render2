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

		int32_t offset{};
	};

	struct Descriptor
	{
		OpenglVAO& VAO;

		std::vector<VertexAttribute> attributes{};
		GLsizei stride{};

		enum class Divisor
		{
			zero,
			one
		} divisor{};

		Descriptor& add(
		    DataType dataType
		);

		GLuint getDivisor() const;

		void finalize(OpenglVBO& VBO);
	};

	struct OpenglVAO
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		std::unordered_map<std::string, Descriptor> descriptors{};
		int attributeCount = 0;

		Descriptor& newDescriptor(std::string_view name, Descriptor::Divisor divisor);

		void bind();

		OpenglVAO(OpenglContext& openglContext);
		~OpenglVAO();
	};
}
