#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <tepp/optional_ref.h>

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
		int divisor{};
	};

	struct Descriptor
	{
		OpenglVAO& VAO;

		std::vector<VertexAttribute> attributes{};
		GLsizei stride{};

		int divisor = 0;

		Descriptor& add(
		    DataType dataType,
		    std::optional<int> divisor = std::nullopt
		);

		GLuint getDivisor() const;

		void finalize(OpenglVBO& VBO);
	};

	struct OpenglVAO
	{
		OpenglContext* openglContext;
		Qualified<GLuint> ID{};

		std::unordered_map<std::string, Descriptor> descriptors{};
		int attributeCount = 0;

		te::optional_ref<OpenglVBO> indicesBuffer{};

		Descriptor& newDescriptor(std::string_view name, int divisor);
		void addQuadDescriptor(std::string_view name, OpenglVBO& VBO);
		void addIndicesBuffer(OpenglVBO& VBO);

		void bind();

		NO_COPY(OpenglVAO);
		DEFAULT_MOVE(OpenglVAO);

		OpenglVAO(OpenglContext& openglContext);
		~OpenglVAO();
	};
}
