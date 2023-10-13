#include "render/opengl/OpenglVAO.h"

#include "render/opengl/OpenglVBO.h"

namespace render::opengl
{
	Descriptor& OpenglVAO::newDescriptor(std::string_view name_) {
		auto name = std::string(name_);
		if (this->descriptors.contains(name)) {
			assert(0);
		}

		auto [it, b] = this->descriptors.emplace(name, Descriptor{ .VAO = *this });

		return it->second;
	}

	void OpenglVAO::bind() {
		this->openglContext.bind(*this);
	}

	OpenglVAO::OpenglVAO(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
		glCreateVertexArrays(1, &this->ID.data);
	}

	OpenglVAO::~OpenglVAO() {
		glDeleteVertexArrays(1, &this->ID.data);
	}

	Descriptor& Descriptor::add(DataType dataType, VertexAttribute::Divisor divisor) {
		this->attributes.push_back(
		    VertexAttribute{
		        .dataType = dataType,
		        .divisor = divisor,
		        .offset = this->stride,
		    }
		);

		this->stride += dataTypeByteSize[dataType];

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

	void Descriptor::finalize(OpenglVBO& VBO) {
		this->VAO.bind();
		VBO.bind(BufferTarget::Type::ARRAY_BUFFER);

		int index = 0;
		for (auto const& attribute : this->attributes) {
			switch (attribute.dataType) {
				case DataType::f32:
					floatVertex(index++, 1, this->stride, (void*)attribute.offset, attribute.getDivisor());
					break;
				case DataType::vec2:
					floatVertex(index++, 2, this->stride, (void*)attribute.offset, attribute.getDivisor());
					break;
				case DataType::vec3:
					floatVertex(index++, 3, this->stride, (void*)attribute.offset, attribute.getDivisor());
					break;
				case DataType::vec4:
					floatVertex(index++, 4, this->stride, (void*)attribute.offset, attribute.getDivisor());
					break;
				case DataType::mat2:
					floatVertex(index++, 2, this->stride, (void*)attribute.offset, attribute.getDivisor());
					floatVertex(index++, 2, this->stride, (void*)(attribute.offset + 2 * sizeof(float)), attribute.getDivisor());
					break;
				case DataType::mat3:
					floatVertex(index++, 3, this->stride, (void*)attribute.offset, attribute.getDivisor());
					floatVertex(index++, 3, this->stride, (void*)(attribute.offset + 3 * sizeof(float)), attribute.getDivisor());
					floatVertex(index++, 3, this->stride, (void*)(attribute.offset + 9 * sizeof(float)), attribute.getDivisor());
					break;
				case DataType::mat4:
					floatVertex(index++, 4, this->stride, (void*)attribute.offset, attribute.getDivisor());
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 4 * sizeof(float)), attribute.getDivisor());
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 8 * sizeof(float)), attribute.getDivisor());
					floatVertex(index++, 4, this->stride, (void*)(attribute.offset + 12 * sizeof(float)), attribute.getDivisor());
					break;
				default:
				case DataType::i32:
				case DataType::ivec2:
				case DataType::ivec3:
				case DataType::ivec4:
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

	GLuint VertexAttribute::getDivisor() const {
		switch (this->divisor) {
			case Divisor::zero:
				return 0;
			case Divisor::one:
				return 1;
			default:
				assert(0);
				return 0;
		}
	}
}