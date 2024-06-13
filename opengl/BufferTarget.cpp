#include "render/opengl/BufferTarget.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	GLenum BufferTarget::get() const {
		static constexpr te::enum_array<Type, GLenum> lookup{
			{ Type::ARRAY_BUFFER, GL_ARRAY_BUFFER },
			{ Type::ATOMIC_COUNTER_BUFFER, GL_ATOMIC_COUNTER_BUFFER },
			{ Type::COPY_READ_BUFFER, GL_COPY_READ_BUFFER },
			{ Type::COPY_WRITE_BUFFER, GL_COPY_WRITE_BUFFER },
			{ Type::DISPATCH_INDIRECT_BUFFER, GL_DISPATCH_INDIRECT_BUFFER },
			{ Type::DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER },
			{ Type::ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER },
			{ Type::PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER },
			{ Type::PIXEL_UNPACK_BUFFER, GL_PIXEL_UNPACK_BUFFER },
			{ Type::SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER },
			{ Type::TEXTURE_BUFFER, GL_TEXTURE_BUFFER },
			{ Type::TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER },
			{ Type::UNIFORM_BUFFER, GL_UNIFORM_BUFFER },

#ifdef WRANGLE_GLESv3
			{ Type::QUERY_BUFFER, -1 },
#else
			{ Type::QUERY_BUFFER, GL_QUERY_BUFFER },
#endif
		};

		auto result = lookup[this->type];
		assert(result != -1);
		return result;
	}

	BufferTarget::BufferTarget(BufferTarget::Type type_)
	    : type(type_) {
	}

	BufferTarget::operator BufferTarget::Type() {
		return this->type;
	}
}