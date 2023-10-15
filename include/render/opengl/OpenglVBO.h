#pragma once

#include <array>
#include <limits>
#include <utility>
#include <vector>

#include <wrangled_gl/wrangled_gl.h>

#include <misc/LimitChecks.h>

#include "render/opengl/BufferTarget.h"
#include "render/opengl/BufferUsageHint.h"
#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglContext;
	struct BufferTarget;

	struct OpenglVBO
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		struct BufferSizeInformation
		{
			int elementByteSize{};
			int elementCount{};

			int getByteSize() const;
		} bufferSizeInformation{};

		template<class T>
		void set(std::vector<T> const& data, BufferUsageHint bufferUsageHint);

		template<class T, size_t N>
		void set(std::array<T, N> const& data, BufferUsageHint bufferUsageHint);

		void bind(BufferTarget bufferTarget);

		OpenglVBO(OpenglContext& openglContext);
		~OpenglVBO();
	};

	template<class T>
	inline void OpenglVBO::set(std::vector<T> const& data, BufferUsageHint bufferUsageHint) {
		misc::abortAssign(this->bufferSizeInformation.elementByteSize, sizeof(T));
		misc::abortAssign(this->bufferSizeInformation.elementCount, data.size());

		this->bind(BufferTarget::Type::ARRAY_BUFFER);

		glBufferData(
		    GL_ARRAY_BUFFER,
		    this->bufferSizeInformation.getByteSize(),
		    data.data(),
		    bufferUsageHint.get()
		);
	}

	template<class T, size_t N>
	inline void OpenglVBO::set(std::array<T, N> const& data, BufferUsageHint bufferUsageHint) {
		misc::abortAssign(this->bufferSizeInformation.elementByteSize, sizeof(T));
		misc::abortAssign(this->bufferSizeInformation.elementCount, data.size());

		this->bind(BufferTarget::Type::ARRAY_BUFFER);

		glBufferData(
		    GL_ARRAY_BUFFER,
		    this->bufferSizeInformation.getByteSize(),
		    data.data(),
		    bufferUsageHint.get()
		);
	}
}
