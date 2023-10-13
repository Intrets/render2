#pragma once

#include <wrangled_gl/wrangled_gl.h>

namespace render::opengl
{
	struct BufferTarget
	{
		enum class Type
		{
			ARRAY_BUFFER,
			ATOMIC_COUNTER_BUFFER,
			COPY_READ_BUFFER,
			COPY_WRITE_BUFFER,
			DISPATCH_INDIRECT_BUFFER,
			DRAW_INDIRECT_BUFFER,
			ELEMENT_ARRAY_BUFFER,
			PIXEL_PACK_BUFFER,
			PIXEL_UNPACK_BUFFER,
			QUERY_BUFFER,
			SHADER_STORAGE_BUFFER,
			TEXTURE_BUFFER,
			TRANSFORM_FEEDBACK_BUFFER,
			UNIFORM_BUFFER,
			MAX
		} type{};

		BufferTarget() = default;
		BufferTarget(BufferTarget::Type type_);
		~BufferTarget() = default;

		operator BufferTarget::Type();

		GLenum get() const;
	};
}
