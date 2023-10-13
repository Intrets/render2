#include "render/opengl/BufferUsageHint.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	BufferUsageHint::BufferUsageHint(Type type_)
	    : type(type_) {
	}

	GLenum BufferUsageHint::get() const {
		constexpr te::enum_array<BufferUsageHint::Type, GLenum> lookup = {
			GL_STREAM_DRAW,
			GL_STREAM_READ,
			GL_STREAM_COPY,
			GL_STATIC_DRAW,
			GL_STATIC_READ,
			GL_STATIC_COPY,
			GL_DYNAMIC_DRAW,
			GL_DYNAMIC_READ,
			GL_DYNAMIC_COPY,
		};

		return lookup[this->type];
	}
}