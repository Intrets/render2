#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <wglm/vec2.hpp>

#include <misc/Misc.h>

#include <render/opengl/Qualifier.h>

#include <span>
#include <filesystem>

namespace render::opengl
{
	struct TextureFormat
	{
		enum class PixelFormat
		{
			R16F,
			R32F,
			RGB32F,
			R16,
			RGB16,
			RGB8,
			MAX
		} pixelFormat{};

		GLint getInternalFormat() const;

		glm::ivec2 size{};
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

		int32_t mipmapLevels = 1;

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

		integer_t getPixelCount() const;
		integer_t getByteSize() const;
		integer_t channelCount() const;
	};

	struct OpenglContext;

	struct Opengl2DTexture
	{
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};
		TextureFormat textureFormat{};
		bool flippedUV = false;
		std::filesystem::path source{};

		void swap(Opengl2DTexture& other);

		void bind();

		void setWrapping(TextureFormat::Wrapping x, TextureFormat::Wrapping y);

		void generateMipmap();

		Opengl2DTexture(OpenglContext& openglContext);
		explicit Opengl2DTexture(OpenglContext& openglContext, GLuint ID);

		NO_COPY(Opengl2DTexture);
		Opengl2DTexture(Opengl2DTexture&&) = default;
		Opengl2DTexture& operator=(Opengl2DTexture&& other);

		Opengl2DTexture() = delete;
		~Opengl2DTexture();

		static std::optional<Opengl2DTexture> make(OpenglContext& openglContext, TextureFormat const& textureFormat, std::optional<std::span<std::byte const>> data);
	};

	struct Opengl2DArrayTexture
	{
		std::reference_wrapper<OpenglContext> openglContext;
		Qualified<GLuint> ID{};
		glm::ivec2 size{};
		int32_t layers{};

		void bind();

		explicit Opengl2DArrayTexture(OpenglContext& openglContext);
		explicit Opengl2DArrayTexture(OpenglContext& openglContext, GLuint ID);

		NO_COPY(Opengl2DArrayTexture);
		DEFAULT_MOVE(Opengl2DArrayTexture);

		Opengl2DArrayTexture() = delete;
		~Opengl2DArrayTexture();

		static std::optional<Opengl2DArrayTexture> make(OpenglContext& openglContext, TextureFormat const& textureFormat);
	};
}