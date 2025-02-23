#include "render/opengl/TextureTarget.h"

#include <tepp/enum_array.h>

namespace render::opengl
{
	GLenum TextureTarget::get() const {
		static constexpr te::enum_array<Type, GLenum> lookup{
			{ Type::UNBOUND, -1 },
			{ Type::TEXTURE_2D, GL_TEXTURE_2D },
			{ Type::TEXTURE_3D, GL_TEXTURE_3D },
			{ Type::TEXTURE_2D_ARRAY, GL_TEXTURE_2D_ARRAY },
			{ Type::TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP },
			{ Type::TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_CUBE_MAP_ARRAY },
			{ Type::TEXTURE_BUFFER, GL_TEXTURE_BUFFER },
			{ Type::TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE },
			{ Type::TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_2D_MULTISAMPLE_ARRAY },

#ifdef WRANGLE_GLESv3
			{ Type::TEXTURE_1D, -1 },
			{ Type::TEXTURE_1D_ARRAY, -1 },
			{ Type::TEXTURE_RECTANGLE, -1 },
#else
			{ Type::TEXTURE_1D, GL_TEXTURE_1D },
			{ Type::TEXTURE_1D_ARRAY, GL_TEXTURE_1D_ARRAY },
			{ Type::TEXTURE_RECTANGLE, GL_TEXTURE_RECTANGLE },
#endif
		};

		auto result = lookup[this->type];
		tassert(result != -1);
		return result;
	}

	bool TextureTarget::unbound() const {
		return this->type == Type::UNBOUND;
	}

	TextureTarget::TextureTarget(TextureTarget::Type type_)
	    : type(type_) {
	}

	TextureTarget::operator TextureTarget::Type() {
		return this->type;
	}
}
