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

		std::optional<Opengl2DTexture> make(OpenglContext& openglContext, TextureFormat const& textureFormat, std::optional<std::span<std::byte>> data);
	};
}