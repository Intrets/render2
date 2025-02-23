#include "render/opengl/OpenglVBO.h"

#include "render/opengl/BufferTarget.h"
#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	void OpenglVBO::tallyBytesTransferred(OpenglContext& openglContext, integer_t bytes) {
		openglContext.tallyBytesTransferred(bytes);
	}

	void OpenglVBO::set(TypeErasedBuffer data, BufferUsageHint bufferUsageHint, BufferTarget bufferTarget) {
		this->bufferSizeInformation = data.bufferSizeInformation;

		this->bind(BufferTarget::Type::ARRAY_BUFFER);

		tallyBytesTransferred(this->openglContext, this->bufferSizeInformation.getByteSize());

		glBufferData(
		    GL_ARRAY_BUFFER,
		    data.data.size_bytes(),
		    data.data.data(),
		    bufferUsageHint.get()
		);
	}

	void OpenglVBO::bind(BufferTarget bufferTarget) {
		this->openglContext.bind(*this, bufferTarget);
	}

	OpenglVBO::OpenglVBO(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
		glGenBuffers(1, &this->ID.data);
	}

	OpenglVBO::~OpenglVBO() {
		if (this->ID && this->ID.data != 0) {
			glDeleteBuffers(1, &this->ID.data);
		}
	}

	integer_t OpenglVBO::BufferSizeInformation::getByteSize() const {
		return this->elementCount * this->elementByteSize;
	}

	GLsizei OpenglVBO::BufferSizeInformation::getGLElementCount() const {
		auto max = std::numeric_limits<GLsizei>::max();
		if (std::cmp_greater(this->elementCount, max)) {
			tassert(0);
			return max;
		}
		else {
			return static_cast<GLsizei>(this->elementCount);
		}
	}

	bool OpenglVBO::BufferSizeInformation::empty() const {
		return this->elementCount == 0;
	}
}