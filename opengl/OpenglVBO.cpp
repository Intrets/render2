#include "render/opengl/OpenglVBO.h"

#include "render/opengl/BufferTarget.h"
#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
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

	int OpenglVBO::BufferSizeInformation::getByteSize() const {
		return this->elementCount * this->elementByteSize;
	}
}