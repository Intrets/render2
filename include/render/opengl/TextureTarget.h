#pragma once

#include <wrangled_gl/wrangled_gl.h>

namespace render::opengl
{
	struct TextureTarget
	{
		enum class Type
		{
			UNBOUND,
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_3D,
			TEXTURE_1D_ARRAY,
			TEXTURE_2D_ARRAY,
			TEXTURE_RECTANGLE,
			TEXTURE_CUBE_MAP,
			TEXTURE_CUBE_MAP_ARRAY,
			TEXTURE_BUFFER,
			TEXTURE_2D_MULTISAMPLE,
			TEXTURE_2D_MULTISAMPLE_ARRAY,
			MAX
		} type{};

		bool unbound() const;

		TextureTarget() = default;
		TextureTarget(TextureTarget::Type type_);
		~TextureTarget() = default;

		operator TextureTarget::Type();

		GLenum get() const;
	};
}
