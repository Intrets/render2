#include "render/opengl/OpenglTexture.h"

#include <tepp/enum_array.h>

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	GLint TextureFormat::getInternalFormat() const {
		constexpr te::enum_array<PixelFormat, GLint> lookup{
			{ PixelFormat::R16F, GL_R16F },
			{ PixelFormat::R32F, GL_R32F },
			{ PixelFormat::RGB32F, GL_RGB32F },
			{ PixelFormat::R16, GL_R16 },
			{ PixelFormat::RGB16, GL_RGB16 },
			{ PixelFormat::RGB8, GL_RGB8 },
			{ PixelFormat::RGBA8, GL_RGBA8 },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataFormat() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::R16F, GL_RED },
			{ PixelFormat::R32F, GL_RED },
			{ PixelFormat::RGB32F, GL_RGB },
			{ PixelFormat::R16, GL_RED },
			{ PixelFormat::RGB16, GL_RGB },
			{ PixelFormat::RGB8, GL_RGB },
			{ PixelFormat::RGBA8, GL_RGBA },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataType() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::R16F, GL_HALF_FLOAT },
			{ PixelFormat::R32F, GL_FLOAT },
			{ PixelFormat::RGB32F, GL_FLOAT },
			{ PixelFormat::R16, GL_UNSIGNED_SHORT },
			{ PixelFormat::RGB16, GL_UNSIGNED_SHORT },
			{ PixelFormat::RGB8, GL_UNSIGNED_BYTE },
			{ PixelFormat::RGBA8, GL_UNSIGNED_BYTE },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getMinFilter() const {
		if (this->mipmapLevels != 1) {
			switch (this->mipmapFiltering) {
				case MipmapFiltering::NEAREST:
				{
					constexpr te::enum_array<Filtering, GLenum> lookup{
						{ Filtering::LINEAR, GL_LINEAR_MIPMAP_NEAREST },
						{ Filtering::NEAREST, GL_NEAREST_MIPMAP_NEAREST },
					};

					return lookup[this->filtering];
				}
				case MipmapFiltering::LINEAR:
				{
					constexpr te::enum_array<Filtering, GLenum> lookup{
						{ Filtering::LINEAR, GL_LINEAR_MIPMAP_LINEAR },
						{ Filtering::NEAREST, GL_NEAREST_MIPMAP_LINEAR },
					};

					return lookup[this->filtering];
				}
				default:
					return GL_LINEAR;
			}
		}
		else {
			constexpr te::enum_array<Filtering, GLenum> lookup{
				{ Filtering::LINEAR, GL_LINEAR },
				{ Filtering::NEAREST, GL_NEAREST },
			};
			return lookup[this->filtering];
		}
	}

	GLenum TextureFormat::getMagFilter() const {
		constexpr te::enum_array<Filtering, GLenum> lookup{
			{ Filtering::LINEAR, GL_LINEAR },
			{ Filtering::NEAREST, GL_NEAREST },
		};

		return lookup[this->filtering];
	}

	GLenum TextureFormat::getWrapping(Wrapping wrapping) {
		constexpr te::enum_array<Wrapping, GLenum> lookup{
			{ Wrapping::REPEAT, GL_REPEAT },
			{ Wrapping::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER },
			{ Wrapping::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			{ Wrapping::MIRRORED_REPEAT, GL_MIRRORED_REPEAT },
		};

		return lookup[wrapping];
	}

	GLenum TextureFormat::getWrappingX() const {
		return getWrapping(this->wrappingX);
	}

	GLenum TextureFormat::getWrappingY() const {
		return getWrapping(this->wrappingY);
	}

	integer_t TextureFormat::getPixelCount() const {
		return static_cast<integer_t>(this->size.x) * this->size.y;
	}

	integer_t TextureFormat::getByteSize() const {
		return this->getPixelCount() * this->getPixelSize();
	}

	integer_t TextureFormat::getPixelSize() const {
		constexpr te::enum_array<PixelFormat, integer_t> lookup{
			{ PixelFormat::R16F, 2 * 1 },
			{ PixelFormat::R32F, 4 * 1 },
			{ PixelFormat::RGB32F, 4 * 3 },
			{ PixelFormat::R16, 2 * 1 },
			{ PixelFormat::RGB16, 2 * 3 },
			{ PixelFormat::RGB8, 1 * 3 },
			{ PixelFormat::RGBA8, 1 * 4 },
		};

		return lookup[this->pixelFormat];
	}

	integer_t TextureFormat::channelCount() const {
		constexpr te::enum_array<PixelFormat, integer_t> lookup{
			{ PixelFormat::R16F, 1 },
			{ PixelFormat::R32F, 1 },
			{ PixelFormat::RGB32F, 3 },
			{ PixelFormat::R16, 1 },
			{ PixelFormat::RGB16, 3 },
			{ PixelFormat::RGB8, 3 },
			{ PixelFormat::RGBA8, 4 },
		};

		return lookup[this->pixelFormat];
	}

	void Opengl2DTexture::swap(Opengl2DTexture& other) {
		assert(&this->openglContext == &other.openglContext);

		std::swap(this->ID, other.ID);
		std::swap(this->textureFormat, other.textureFormat);
		std::swap(this->flippedUV, other.flippedUV);
		std::swap(this->source, other.source);
	}

	void Opengl2DTexture::bind() {
		this->openglContext.bind(*this);
	}

	void Opengl2DTexture::setWrapping(TextureFormat::Wrapping x, TextureFormat::Wrapping y) {
		if (this->textureFormat.wrappingX != x || this->textureFormat.wrappingY != y) {
			this->bind();
			this->textureFormat.wrappingX = x;
			this->textureFormat.wrappingY = y;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->textureFormat.getWrappingX());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->textureFormat.getWrappingY());
		}
	}

	void Opengl2DTexture::generateMipmap() {
		this->bind();
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	std::vector<std::byte> Opengl2DTexture::download(TextureFormat& targetFormat) {
		this->bind();

		targetFormat.size = this->textureFormat.size;
		targetFormat.layers = 0;

		std::vector<std::byte> result{};
		result.resize(targetFormat.getByteSize());

		glGetTexImage(
		    GL_TEXTURE_2D,
		    0,
		    targetFormat.getPixelDataFormat(),
		    targetFormat.getPixelDataType(),
		    result.data()
		);

		return result;
	}

	Opengl2DTexture::Opengl2DTexture(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
	}

	Opengl2DTexture::Opengl2DTexture(OpenglContext& openglContext_, GLuint ID_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.getQualifier();
		this->ID.data = ID_;
	}

	Opengl2DTexture& Opengl2DTexture::operator=(Opengl2DTexture&& other) {
		assert(&this->openglContext == &other.openglContext);

		glDeleteTextures(1, &this->ID.data);

		this->ID = other.ID;
		other.ID.clear();

		this->textureFormat = other.textureFormat;
		this->flippedUV = other.flippedUV;

		return *this;
	}

	Opengl2DTexture::~Opengl2DTexture() {
		glDeleteTextures(1, &this->ID.data);
	}

	std::optional<Opengl2DTexture> Opengl2DTexture::make(OpenglContext& openglContext, TextureFormat const& textureFormat, std::optional<std::span<std::byte const>> data) {
		int32_t maxSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		if (textureFormat.size.x > maxSize || textureFormat.size.y > maxSize) {
			openglContext.logError("Tried to make texture with size {} {}, maximum size supported is {}.\n", textureFormat.size.x, textureFormat.size.y, maxSize);
			return std::nullopt;
		}

		auto result = Opengl2DTexture(openglContext);
		glGenTextures(1, &result.ID.data);
		result.bind();

		result.textureFormat = textureFormat;

		void const* ptr = nullptr;

		if (data.has_value() && !data->empty()) {
			if (std::cmp_not_equal(textureFormat.getByteSize(), data->size())) {
				openglContext.logError("Mismatched byte size when trying to load texture. Wanted {}, have {}.\n", textureFormat.getByteSize(), data->size());

				return std::nullopt;
			}

			ptr = data->data();
		}

		glTexImage2D(
		    GL_TEXTURE_2D,
		    0,
		    textureFormat.getInternalFormat(),
		    textureFormat.size.x,
		    textureFormat.size.y,
		    0,
		    textureFormat.getPixelDataFormat(),
		    textureFormat.getPixelDataType(),
		    ptr
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFormat.getMagFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFormat.getMinFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureFormat.getWrappingX());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureFormat.getWrappingY());

		if (textureFormat.mipmapLevels > 1) {
			result.generateMipmap();
		}

		return result;
	}

	void Opengl2DArrayTexture::bind() {
		this->openglContext.get().bind(*this);
	}

	Opengl2DArrayTexture::Opengl2DArrayTexture(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
		this->ID.qualifier = this->openglContext.get().getQualifier();
	}

	Opengl2DArrayTexture::Opengl2DArrayTexture(OpenglContext& openglContext_, GLuint ID)
	    : openglContext(openglContext_) {
		this->ID.data = ID;
		this->ID.qualifier = this->openglContext.get().getQualifier();
	}

	Opengl2DArrayTexture::~Opengl2DArrayTexture() {
	}

	std::optional<Opengl2DArrayTexture> Opengl2DArrayTexture::make(OpenglContext& openglContext, TextureFormat const& textureFormat) {
		int32_t maxSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		if (textureFormat.size.x > maxSize || textureFormat.size.y > maxSize) {
			openglContext.logError("Tried to make texture with size {} {}, maximum size supported is {}.\n", textureFormat.size.x, textureFormat.size.y, maxSize);
			return std::nullopt;
		}

		auto result = Opengl2DArrayTexture(openglContext);
		glGenTextures(1, &result.ID.data);
		result.bind();

		result.size = textureFormat.size;
		result.layers = textureFormat.layers;

		for (int i = 0; i < textureFormat.mipmapLevels; i++) {
			glTexImage3D(
			    GL_TEXTURE_2D_ARRAY,
			    i,
			    textureFormat.getInternalFormat(),
			    textureFormat.size.x >> i,
			    textureFormat.size.y >> i,
			    textureFormat.layers,
			    0,
			    textureFormat.getPixelDataFormat(),
			    textureFormat.getPixelDataType(),
			    nullptr
			);

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, textureFormat.getMagFilter());
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, textureFormat.getMinFilter());
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, textureFormat.getWrappingX());
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, textureFormat.getWrappingY());
		}

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, textureFormat.mipmapLevels - 1);

		return result;
	}
}
