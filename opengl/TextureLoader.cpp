#include "render/opengl/TextureLoader.h"

#include <wrangled_gl/wrangled_gl.h>

#if defined(COMPILER_CLANGCL) || defined(COMPILER_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wdeprecated-anon-enum-enum-conversion"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wunused-variable"
#elif defined(COMPILER_MSVC)
#pragma warning(push, 0)
#pragma warning(disable : 4201; disable : 4324; disable : 4310)
#endif

#include <gli/gli.hpp>

#if defined(COMPILER_CLANGCL) || defined(COMPILER_CLANG)
#pragma clang diagnostic pop
#elif defined(COMPILER_MSVC)
#pragma warning(pop)
#endif

#include <ranges>

#include "render/opengl/OpenglContext.h"
#include "render/opengl/OpenglTexture.h"

#include <tepp/safety_cast.h>

namespace render::opengl
{
	Opengl2DTexture load2DTexture(OpenglContext& openglContext, std::span<char const> buffer, bool SRGB) {
		gli::texture Texture = gli::load_dds(buffer.data(), buffer.size());

		if (Texture.target() == gli::texture::target_type::TARGET_2D) {
			if (auto result = impl::loadTexture(openglContext, Texture, SRGB)) {
				if (auto t = std::get_if<Opengl2DTexture>(&result.value())) {
					return std::move(*t);
				}
			}
		}

		return Opengl2DTexture(openglContext);
	}

	Opengl2DArrayTexture load2DArrayTexture(OpenglContext& openglContext, std::span<char const> buffer, bool SRGB) {
		gli::texture Texture = gli::load_dds(buffer.data(), buffer.size());

		if (Texture.target() == gli::texture::target_type::TARGET_2D_ARRAY) {
			if (auto result = impl::loadTexture(openglContext, Texture, SRGB)) {
				if (auto t = std::get_if<Opengl2DArrayTexture>(&result.value())) {
					return std::move(*t);
				}
			}
		}

		return Opengl2DArrayTexture(openglContext);
	}

	impl::LoadTextureResult impl::loadTexture(
	    OpenglContext& openglContext,
	    gli::texture const& Texture, bool SRGB
	) {
		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		auto [internalFormat, externalFormat] = [&]() -> std::pair<GLenum, GLenum> {
			switch (Texture.format()) {
#ifndef WRANGLE_GLESv3
				case gli::format::FORMAT_RGB_DXT1_UNORM_BLOCK8:
					return { GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, 0 };
				case gli::format::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
					return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, 0 };
				case gli::format::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
					return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, 0 };
				case gli::format::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
					return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, 0 };
#endif

				case gli::format::FORMAT_RGB_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
				case gli::format::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
					tassert(0);
				case gli::format::FORMAT_BGR8_UNORM_PACK8:
					if (SRGB) {
						return { GL_SRGB8, Format.External };
					}
				default:
					return { Format.Internal, Format.External };
			}
		}();

		GLenum Target = GL.translate(Texture.target());

		glm::tvec3<GLsizei> const Extent1(Texture.extent());
		GLsizei const FaceTotal = te::safety_cast<GLsizei>(Texture.layers() * Texture.faces());

		glm::ivec2 size{};

		size.x = Extent1.x;
		size.y = Texture.target() == gli::TARGET_2D || Texture.target() == gli::TARGET_2D_ARRAY ? Extent1.y : FaceTotal;

		auto result = [&]() -> std::optional<std::variant<Opengl2DTexture, Opengl2DArrayTexture>> {
			if (Target == GL_TEXTURE_2D) {
				auto result = Opengl2DTexture(openglContext);
				result.textureFormat.size = size;
				glGenTextures(1, &result.ID.data);
				result.bind();
				return result;
			}
			else if (Target == GL_TEXTURE_2D_ARRAY) {
				auto result = Opengl2DArrayTexture(openglContext);
				result.size = size;
				result.layers = te::safety_cast<int32_t>(Texture.layers());
				glGenTextures(1, &result.ID.data);
				result.bind();
				return result;
			}
			else {
				return std::nullopt;
			}
		}();

		if (!result.has_value()) {
			return std::nullopt;
		}

		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, te::safety_cast<GLint>(Texture.levels() - 1));
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		switch (Texture.target()) {
			case gli::TARGET_1D_ARRAY:
			case gli::TARGET_2D:
			case gli::TARGET_CUBE:
				glTexStorage2D(
				    Target, te::safety_cast<GLint>(Texture.levels()), internalFormat,
				    Extent1.x, Texture.target() == gli::TARGET_2D ? Extent1.y : FaceTotal
				);
				break;
			case gli::TARGET_2D_ARRAY:
				glTexStorage3D(
				    Target, te::safety_cast<GLint>(Texture.levels()), internalFormat,
				    Extent1.x, Extent1.y, te::safety_cast<GLsizei>(Texture.layers())
				);
				break;
			case gli::TARGET_1D:
			case gli::TARGET_3D:
			case gli::TARGET_CUBE_ARRAY:
			default:
				tassert(0);
				break;
		}

		auto layers = Texture.layers();
		auto faces = Texture.faces();
		auto levels = Texture.levels();

		for (std::size_t Layer = 0; Layer < layers; ++Layer) {
			for (std::size_t Face = 0; Face < faces; ++Face) {
				for (std::size_t Level = 0; Level < levels; ++Level) {
					GLsizei const LayerGL = te::safety_cast<GLsizei>(Layer);
					glm::tvec3<GLsizei> Extent(Texture.extent(Level));
					Target = gli::is_target_cube(Texture.target())
					             ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
					             : Target;

					switch (Texture.target()) {
						case gli::TARGET_1D_ARRAY:
						case gli::TARGET_2D:
						case gli::TARGET_CUBE:
						{
							if (gli::is_compressed(Texture.format())) {
								glCompressedTexSubImage2D(
								    Target, te::safety_cast<GLint>(Level),
								    0, 0,
								    Extent.x, Extent.y,
								    internalFormat,
								    te::safety_cast<GLsizei>(Texture.size(Level)),
								    Texture.data(Layer, Face, Level)
								);
							}
							else {
								glTexSubImage2D(
								    Target, te::safety_cast<GLint>(Level),
								    0, 0,
								    Extent.x,
								    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
								    externalFormat, Format.Type,
								    Texture.data(Layer, Face, Level)
								);
							}
						} break;
						case gli::TARGET_2D_ARRAY:
						{
							if (gli::is_compressed(Texture.format())) {
								glCompressedTexSubImage3D(
								    Target, te::safety_cast<GLint>(Level),
								    0, 0, te::safety_cast<GLint>(Layer),
								    Extent.x, Extent.y, 1,
								    internalFormat,
								    te::safety_cast<GLsizei>(Texture.size(Level)),
								    Texture.data(Layer, Face, Level)
								);
							}
							else {
								glTexSubImage3D(
								    Target, te::safety_cast<GLint>(Level),
								    0, 0, te::safety_cast<GLint>(Layer),
								    Extent.x, Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y, 1,
								    externalFormat, Format.Type,
								    Texture.data(Layer, Face, Level)
								);
							}
						} break;
						case gli::TARGET_1D:
						case gli::TARGET_3D:
						case gli::TARGET_CUBE_ARRAY:
						default:
							tassert(0);
							break;
					}
				}
			}
		}

		return result;
	}
}
