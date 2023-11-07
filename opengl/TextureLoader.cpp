#include "render/opengl/TextureLoader.h"

#include <wrangled_gl/wrangled_gl.h>

#include <gli/gli.hpp>

#include <resources/Resources.h>

#include "render/opengl/OpenglTexture.h"

namespace render::opengl
{
	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer) {
		auto span = buffer.data<char>();
		gli::texture Texture = gli::load_dds(span.data(), span.size());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		GLenum Target = GL.translate(Texture.target());

		auto result = Opengl2DTexture(openglContext);
		result.flippedUV = true;
		glGenTextures(1, &result.ID.data);
		if (Target != GL_TEXTURE_2D) {
			return result;
		}

		result.bind();
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glm::tvec3<GLsizei> const Extent1(Texture.extent());
		GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

		switch (Texture.target()) {
			case gli::TARGET_1D_ARRAY:
			case gli::TARGET_2D:
			case gli::TARGET_CUBE:
				glTexStorage2D(
				    Target, static_cast<GLint>(Texture.levels()), GL_SRGB8_ALPHA8,
				    Extent1.x, Texture.target() == gli::TARGET_2D ? Extent1.y : FaceTotal
				);
				break;
			case gli::TARGET_1D:
			case gli::TARGET_2D_ARRAY:
			case gli::TARGET_3D:
			case gli::TARGET_CUBE_ARRAY:
			default:
				assert(0);
				break;
		}

		glm::ivec2 size;

		size.x = Extent1.x;
		size.y = Texture.target() == gli::TARGET_2D ? Extent1.y : FaceTotal;

		for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer) {
			for (std::size_t Face = 0; Face < Texture.faces(); ++Face) {
				for (std::size_t Level = 0; Level < Texture.levels(); ++Level) {
					GLsizei const LayerGL = static_cast<GLsizei>(Layer);
					glm::tvec3<GLsizei> Extent(Texture.extent(Level));
					Target = gli::is_target_cube(Texture.target())
					             ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
					             : Target;

					if (gli::is_compressed(Texture.format())) {
						glCompressedTexSubImage2D(
						    Target, static_cast<GLint>(Level),
						    0, 0,
						    Extent.x,
						    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
						    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
						    Texture.data(Layer, Face, Level)
						);
					}
					else {
						glTexSubImage2D(
						    Target, static_cast<GLint>(Level),
						    0, 0,
						    Extent.x,
						    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
						    Format.External, Format.Type,
						    Texture.data(Layer, Face, Level)
						);
					}
				}
			}
		}

		result.size = { size.x, size.y };

		return result;
	}

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Resource resource) {
		auto buffer = resource.getBuffer();

		if (buffer.has_value()) {
			return load2DTexture(openglContext, *buffer.value());
		}

		return Opengl2DTexture(openglContext);
	}
}
