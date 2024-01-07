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

#include <resources/Resources.h>

#include "render/opengl/OpenglTexture.h"

namespace render::opengl
{
	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer, bool SRGB) {
		auto span = buffer.data<char>();
		gli::texture Texture = gli::load_dds(span.data(), span.size());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		auto compressedFormat = [&]() -> GLenum {
			switch (Texture.format()) {
				case gli::format::FORMAT_RGB_DXT1_UNORM_BLOCK8:
					return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
				case gli::format::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
				case gli::format::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
				case gli::format::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
					return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

				case gli::format::FORMAT_RGB_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
				case gli::format::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
					assert(0);
				default:
					return static_cast<GLenum>(Format.External);
			}
		}();

		auto internalFormat = [&]() -> GLenum {
			switch (Texture.format()) {
				case gli::format::FORMAT_RGB_DXT1_UNORM_BLOCK8:
					return GL_RGB;
				case gli::format::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
				case gli::format::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
					return GL_RGBA;
				case gli::format::FORMAT_BGR8_UNORM_PACK8:
					if (SRGB) {
						return GL_SRGB8;
					}
				case gli::format::FORMAT_RGB_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
				case gli::format::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
				case gli::format::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
				default:
					return static_cast<GLenum>(Format.Internal);
			}
		}();

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
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

		glm::tvec3<GLsizei> const Extent1(Texture.extent());
		GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

		switch (Texture.target()) {
			case gli::TARGET_1D_ARRAY:
			case gli::TARGET_2D:
			case gli::TARGET_CUBE:
				glTexStorage2D(
				    Target, static_cast<GLint>(Texture.levels()), internalFormat,
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
						glCompressedTexImage2D(
						    Target, static_cast<GLint>(Level),
						    compressedFormat,
						    Extent.x, Extent.y,
						    0, static_cast<GLsizei>(Texture.size(Level)),
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

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Resource resource, bool SRGB) {
		auto buffer = resource.getBuffer();

		if (buffer.has_value()) {
			return load2DTexture(openglContext, *buffer.value(), SRGB);
		}

		return Opengl2DTexture(openglContext);
	}

	Opengl2DTexture load2DTextureMipmaps(OpenglContext& openglContext, std::vector<resources::Resource*> resources) {
		auto firstBuffer = resources.front()->getBuffer();
		auto firstSpan = firstBuffer.value()->data<char>();
		gli::texture Texture = gli::load_dds(firstSpan.data(), firstSpan.size());

		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		GLenum Target = GL.translate(Texture.target());

		auto result = Opengl2DTexture(openglContext);
		result.flippedUV = true;
		glGenTextures(1, &result.ID.data);

		result.bind();
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(resources.size()));
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

		glm::ivec2 size;

		size.x = Texture.extent().x;
		size.y = Texture.extent().y;

		glTexStorage2D(
		    Target,
		    static_cast<GLsizei>(resources.size()),
		    GL_SRGB8_ALPHA8,
		    size.x, size.y
		);

		if (gli::is_compressed(Texture.format())) {
			glCompressedTexSubImage2D(
			    Target, static_cast<GLint>(0),
			    0, 0,
			    size.x,
			    size.y,
			    Format.Internal, static_cast<GLsizei>(Texture.size(0)),
			    Texture.data(0, 0, 0)
			);
		}
		else {
			glTexSubImage2D(
			    Target, static_cast<GLint>(0),
			    0, 0,
			    size.x,
			    size.y,
			    Format.External, Format.Type,
			    Texture.data(0, 0, 0)
			);
		}

		int i = 1;
		for (auto& resource : resources | std::views::drop(1)) {
			auto buffer = resource->getBuffer();
			auto span = buffer.value()->data<char>();
			gli::texture Texture2 = gli::load_dds(span.data(), span.size());

			auto Format2 = GL.translate(Texture2.format(), Texture2.swizzles());

			size.x = Texture2.extent().x;
			size.y = Texture2.extent().y;

			if (gli::is_compressed(Texture2.format())) {
				glCompressedTexSubImage2D(
				    Target, static_cast<GLint>(i),
				    0, 0,
				    size.x,
				    size.y,
				    Format2.Internal, static_cast<GLsizei>(Texture2.size(0)),
				    Texture2.data(0, 0, 0)
				);
			}
			else {
				glTexSubImage2D(
				    Target, static_cast<GLint>(i),
				    0, 0,
				    size.x,
				    size.y,
				    Format2.External, Format2.Type,
				    Texture2.data(0, 0, 0)
				);
			}

			i++;
		}

		result.size = { size.x, size.y };

		return result;
	}
}
