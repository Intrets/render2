#pragma once

#include <wrangled_gl/wrangled_gl.h>

namespace render::opengl
{
	struct BufferUsageHint
	{
		enum class Type
		{
			STREAM_DRAW,
			STREAM_READ,
			STREAM_COPY,
			STATIC_DRAW,
			STATIC_READ,
			STATIC_COPY,
			DYNAMIC_DRAW,
			DYNAMIC_READ,
			DYNAMIC_COPY,
			MAX
		} type{};

		BufferUsageHint() = default;
		BufferUsageHint(Type type_);
		~BufferUsageHint() = default;

		GLenum get() const;
	};
}
