#include "render/opengl/OpenglVAO.h"

#include "render/opengl/OpenglVBO.h"

#include <wglm/glm.hpp>

#include <wrangled_gl/wrangled_gl.h>

namespace render::opengl
{
	Descriptor& OpenglVAO::newDescriptor(std::string_view name_, integer_t divisor) {
		auto name = std::string(name_);
		if (this->descriptors.contains(name)) {
			assert(0);
		}

		auto [it, b] = this->descriptors.emplace(name, Descriptor{ .VAO = *this, .divisor = divisor });

		return it->second;
	}

	void OpenglVAO::addQuadDescriptor(std::string_view name, OpenglVBO& VBO) {
		this->newDescriptor(name, 0)
		    .add(render::DataType::vec2)
		    .finalize(VBO);

		constexpr std::array<glm::vec2, 6> quadVertices{ {
			{ 0, 0 },
			{ 1, 0 },
			{ 1, 1 },

			{ 0, 0 },
			{ 1, 1 },
			{ 0, 1 },
		} };

		VBO.set(std::span(quadVertices.begin(), 6), BufferUsageHint::Type::STATIC_DRAW);
	}

	void OpenglVAO::addIndicesBuffer(OpenglVBO& VBO) {
		this->indicesBuffer = VBO;
	}

	void OpenglVAO::bind() {
		this->openglContext->bind(*this);
	}

	OpenglVAO::OpenglVAO(OpenglContext& openglContext_)
	    : openglContext(&openglContext_) {
		this->ID.qualifier = this->openglContext->getQualifier();
#ifdef WRANGLE_GLESv3
        glGenVertexArrays(1, &this->ID.data);
#else
		glCreateVertexArrays(1, &this->ID.data);
#endif
	}

	OpenglVAO::~OpenglVAO() {
		if (this->ID && this->ID.data != 0) {
			glDeleteVertexArrays(1, &this->ID.data);
		}
	}

	Descriptor& Descriptor::add(DataType dataType, std::optional<GLuint> divisor_) {
		integer_t strideMultiplier = 1;
		if (divisor_.has_value()) {
			if (this->divisor > divisor_.value()) {
				strideMultiplier = this->divisor / divisor_.value();
			}
		}

		this->attributes.push_back(
		    VertexAttribute{
		        .dataType = dataType,
		        .offset = this->stride,
		        .divisor = divisor_.value_or(this->divisor),
		    }
		);

		this->stride += static_cast<GLsizei>(dataTypeByteSize[dataType] * strideMultiplier);

		return *this;
	}

	void floatVertex(GLint index, GLint size, GLsizei stride, void* offset, GLuint divisor) {
		glVertexAttribPointer(
		    index,
		    size,
		    GL_FLOAT,
		    GL_FALSE,
		    stride,
		    offset
		);
		glVertexAttribDivisor(index, divisor);
		glEnableVertexAttribArray(index);
	}

	void intVertex(GLint index, GLint size, GLsizei stride, void* offset, GLuint divisor) {
		glVertexAttribIPointer(
		    index,
		    size,
		    GL_INT,
		    stride,
		    offset
		);
		glVertexAttribDivisor(index, divisor);
		glEnableVertexAttribArray(index);
	}

	void int16Vertex(GLint index, GLint size, GLsizei stride, void* offset, GLuint divisor) {
		glVertexAttribIPointer(
		    index,
		    size,
		    GL_SHORT,
		    stride,
		    offset
		);
		glVertexAttribDivisor(index, divisor);
		glEnableVertexAttribArray(index);
	}

	void Descriptor::finalize(OpenglVBO& VBO) {
		this->VAO.bind();
		VBO.bind(BufferTarget::Type::ARRAY_BUFFER);

		GLint& index = this->VAO.attributeCount;
		for (auto const& attribute : this->attributes) {
			switch (attribute.dataType) {
				case DataType::f32:
					floatVertex(index++, 1, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::vec2:
					floatVertex(index++, 2, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::vec3:
					floatVertex(index++, 3, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::vec4:
					floatVertex(index++, 4, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::mat2:
					floatVertex(index++, 2, this->stride, (void*)attribute.offset, attribute.divisor);
					floatVertex(index++, 2, this->stride, (void*)(attribute.offset + 2 * sizeof(float)), attribute.divisor);
					break;
				case DataType::mat3:
					floatVertex(index++, 3, this->stride, (void*)attribute.offset, attribute.divisor);
					floatVertex(index++, 3, this->stride, (void*)(attribute.offset + 3 * sizeof(float)), attribute.divisor);
					floatVertex(index++, 3, this->stride, (void*)(attribute.offset + 9 * sizeof(float)), attribute.divisor);
					break;
				case DataType::mat4:
					floatVertex(index++, 4, this->stride, (void*)attribute.offset, attribute.divisor);
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 4 * sizeof(float)), attribute.divisor);
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 8 * sizeof(float)), attribute.divisor);
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 12 * sizeof(float)), attribute.divisor);
					break;
				case DataType::i32:
					intVertex(index++, 1, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::i16vec2:
					int16Vertex(index++, 2, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::ivec2:
					intVertex(index++, 2, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::ivec3:
					intVertex(index++, 3, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::ivec4:
					intVertex(index++, 4, this->stride, (void*)attribute.offset, attribute.divisor);
					break;
				case DataType::coloru32:
				{
					auto i = index++;
					glVertexAttribPointer(
					    i,
					    static_cast<GLint>(4),
					    GL_UNSIGNED_BYTE,
					    GL_TRUE,
					    this->stride,
					    (void*)attribute.offset
					);
					glVertexAttribDivisor(i, attribute.divisor);
					glEnableVertexAttribArray(i);
				} break;
				default:
				case DataType::u32:
				case DataType::uvec2:
				case DataType::uvec3:
				case DataType::uvec4:
				case DataType::mat2x3:
				case DataType::mat3x2:
				case DataType::mat2x4:
				case DataType::mat4x2:
				case DataType::mat3x4:
				case DataType::mat4x3:
					assert(0);
					break;
			}
		}
	}

	GLuint Descriptor::getDivisor() const {
		return static_cast<GLuint>(this->divisor);
	}
}