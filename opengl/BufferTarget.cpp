#include "render/opengl/BufferTarget.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	GLenum BufferTarget::get() const {
		static constexpr te::enum_array<Type, GLenum> lookup{
			GL_ARRAY_BUFFER,
			GL_ATOMIC_COUNTER_BUFFER,
			GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			GL_DISPATCH_INDIRECT_BUFFER,
			GL_DRAW_INDIRECT_BUFFER,
			GL_ELEMENT_ARRAY_BUFFER,
			GL_PIXEL_PACK_BUFFER,
			GL_PIXEL_UNPACK_BUFFER,
//			GL_QUERY_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
			GL_TEXTURE_BUFFER,
			GL_TRANSFORM_FEEDBACK_BUFFER,
			GL_UNIFORM_BUFFER
		};

		return lookup[this->type];
	}

	BufferTarget::BufferTarget(BufferTarget::Type type_)
	    : type(type_) {
	}

	BufferTarget::operator BufferTarget::Type() {
		return this->type;
	}
}