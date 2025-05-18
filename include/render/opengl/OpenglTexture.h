#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <wglm/vec2.hpp>

#include <misc/Misc.h>

#include <render/opengl/Qualifier.h>

#include <filesystem>
#include <span>

namespace render::opengl
{
#define COMMA(X) X,
#define SETTER(X) \
	void X() { \
		this->data = enum_name::X; \
	}

#define MAKE_CHOICE_ENUM_STRUCT(TYPE, NAME, L) \
	struct TYPE##_struct \
	{ \
		enum class TYPE##_enum{ \
			L(COMMA) MAX \
		} data{}; \
		using enum_name = TYPE##_enum; \
		L(SETTER) \
		enum_name get() const { \
			return data; \
		} \
		operator enum_name() const { \
			return data; \
		} \
		TYPE##_struct& operator=(enum_name v) { \
			this->data = v; \
			return *this; \
		} \
	} NAME{}; \
	using TYPE = TYPE##_struct::TYPE##_enum;

	struct TextureFormat
	{
#define LIST(X) \
	X(RGBA8) \
	X(R16F) \
	X(R32F) \
	X(RGB32F) \
	X(R16) \
	X(RGB16) \
	X(RGB8)

		MAKE_CHOICE_ENUM_STRUCT(PixelFormat, pixelFormat, LIST)

		GLint getInternalFormat() const;

		glm::ivec2 size{};

		GLsizei getWidth() const;
		GLsizei getHeight() const;

		int32_t layers{};

		GLenum getPixelDataFormat() const;
		GLenum getPixelDataType() const;

#undef LIST
#define LIST(X) \
	X(NEAREST) \
	X(LINEAR)
		MAKE_CHOICE_ENUM_STRUCT(Filtering, filtering, LIST)

		GLenum getMinFilter() const;
		GLenum getMagFilter() const;

		int32_t mipmapLevels = 1;

#undef LIST
#define LIST(X) \
	X(NEAREST) \
	X(LINEAR)
		MAKE_CHOICE_ENUM_STRUCT(MipmapFiltering, mipmapFiltering, LIST)

#undef LIST
#define LIST(X) \
	X(REPEAT) \
	X(MIRRORED_REPEAT) \
	X(CLAMP_TO_EDGE) \
	X(CLAMP_TO_BORDER)
		MAKE_CHOICE_ENUM_STRUCT(Wrapping, wrappingX, LIST)
		Wrapping_struct wrappingY{};

		static GLenum getWrapping(Wrapping wrapping);

		GLenum getWrappingX() const;
		GLenum getWrappingY() const;

		integer_t getPixelCount() const;
		integer_t getByteSize() const;
		integer_t getPixelSize() const;
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
		void refreshFiltering();

		std::vector<std::byte> download(TextureFormat& targetFormat);

		Opengl2DTexture(OpenglContext& openglContext);
		explicit Opengl2DTexture(OpenglContext& openglContext, GLuint ID);

		NO_COPY(Opengl2DTexture);
		Opengl2DTexture(Opengl2DTexture&& other);
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

#undef COMMA
#undef SETTER