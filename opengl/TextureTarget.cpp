#include "render/opengl/TextureTarget.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	GLenum TextureTarget::get() const {
		static constexpr te::enum_array<Type, GLenum> lookup{
//			GL_TEXTURE_1D,
			GL_TEXTURE_2D,
			GL_TEXTURE_3D,
//			GL_TEXTURE_1D_ARRAY,
			GL_TEXTURE_2D_ARRAY,
//			GL_TEXTURE_RECTANGLE,
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_CUBE_MAP_ARRAY,
			GL_TEXTURE_BUFFER,
			GL_TEXTURE_2D_MULTISAMPLE,
			GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
		};

		return lookup[this->type];
	}

	TextureTarget::TextureTarget(TextureTarget::Type type_)
	    : type(type_) {
	}

	TextureTarget::operator TextureTarget::Type() {
		return this->type;
	}
}
