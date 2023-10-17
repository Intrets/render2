#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <vec/ivec2.h>

#include <misc/Misc.h>

#include <render/opengl/Qualifier.h>

namespace render::opengl
{
	struct TextureFormat
	{
		enum class PixelFormat
		{
			RGBA,
			RGBA16,
			RGBA32,
			MAX
		} pixelFormat{};

		GLint getInternalFormat() const;

		vec::ivec2 size{};
		int32_t layers{};

		GLenum getPixelDataFormat() const;
		GLenum getPixelDataType() const;

		enum class Filtering
		{
			NEAREST,
			LINEAR,
			MAX
		} filtering{};

		GLenum getMinFilter() const;
		GLenum getMagFilter() const;

		std::optional<vec::ivec2> mipmapLimit{};

		enum class MipmapFiltering
		{
			NEAREST,
			LINEAR,
			MAX
		} mipmapFiltering{};

		enum class Wrapping
		{
			REPEAT,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE,
			CLAMP_TO_BORDER,
			MAX
		};

		Wrapping wrappingX{};
		Wrapping wrappingY{};

		static GLenum getWrapping(Wrapping wrapping);

		GLenum getWrappingX() const;
		GLenum getWrappingY() const;

		int64_t getByteSize() const;
	};

	struct OpenglContext;

	struct Opengl2DTexture
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};
		vec::ivec2 size{};

		void bind();

		Opengl2DTexture(OpenglContext& openglContext);
		explicit Opengl2DTexture(OpenglContext& openglContext, GLuint ID);

		NO_COPY(Opengl2DTexture);
		DEFAULT_MOVE(Opengl2DTexture);

		Opengl2DTexture() = delete;
		~Opengl2DTexture();

		static std::optional<Opengl2DTexture> make(OpenglContext& openglContext, TextureFormat const& textureFormat, std::optional<std::span<std::byte>> data);
	};

	struct Opengl2DArrayTexture
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};
		vec::ivec2 size{};
		int32_t layers{};

		void bind();

		Opengl2DArrayTexture(OpenglContext& openglContext);
		explicit Opengl2DArrayTexture(OpenglContext& openglContext, GLuint ID);

		NO_COPY(Opengl2DArrayTexture);
		DEFAULT_MOVE(Opengl2DArrayTexture);

		Opengl2DArrayTexture() = delete;
		~Opengl2DArrayTexture();

		static std::optional<Opengl2DArrayTexture> make(OpenglContext& openglContext, TextureFormat const& textureFormat);
	};
}