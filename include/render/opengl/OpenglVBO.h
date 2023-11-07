#pragma once

#include <array>
#include <limits>
#include <utility>
#include <vector>
#include <span>

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
		void set(std::span<T const> data, BufferUsageHint bufferUsageHint, BufferTarget bufferTarget = BufferTarget::Type::ARRAY_BUFFER);

		template<class T>
		void set(std::vector<T> const& data, BufferUsageHint bufferUsageHint);

		template<class T, size_t N>
		void set(std::array<T, N> const& data, BufferUsageHint bufferUsageHint);

		void bind(BufferTarget bufferTarget);

		NO_COPY_MOVE(OpenglVBO);

		OpenglVBO(OpenglContext& openglContext);
		~OpenglVBO();
	};

	template<class T>
	inline void OpenglVBO::set(std::span<T const> data, BufferUsageHint bufferUsageHint, BufferTarget bufferTarget) {
		misc::abortAssign(this->bufferSizeInformation.elementByteSize, sizeof(T));
		misc::abortAssign(this->bufferSizeInformation.elementCount, data.size());

		this->bind(bufferTarget);

		glBufferData(
		    bufferTarget.get(),
		    this->bufferSizeInformation.getByteSize(),
		    data.data(),
		    bufferUsageHint.get()
		);
	}

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
