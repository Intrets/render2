#include "render/opengl/OpenglTexture.h"

#include <tepp/enum_array.h>

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	GLint TextureFormat::getInternalFormat() const {
		constexpr te::enum_array<PixelFormat, GLint> lookup{
			{ PixelFormat::R, GL_R8 },
			{ PixelFormat::R16F, GL_R16F },
			{ PixelFormat::R16, GL_R16 },
			{ PixelFormat::RGB16, GL_RGB16 },
			{ PixelFormat::RGBA, GL_RGBA8 },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataFormat() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::R, GL_RED },
			{ PixelFormat::R16F, GL_RED },
			{ PixelFormat::R16, GL_RED },
			{ PixelFormat::RGB16, GL_RGB },
			{ PixelFormat::RGBA, GL_RGBA },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataType() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::R, GL_UNSIGNED_BYTE },
			{ PixelFormat::R16F, GL_HALF_FLOAT },
			{ PixelFormat::R16, GL_UNSIGNED_SHORT },
			{ PixelFormat::RGB16, GL_UNSIGNED_SHORT },
			{ PixelFormat::RGBA, GL_UNSIGNED_BYTE },
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

	integer_t TextureFormat::getByteSize() const {
		integer_t pixelCount = static_cast<integer_t>(this->size.x) * this->size.y;

		constexpr te::enum_array<PixelFormat, integer_t> lookup{
			{ PixelFormat::R, 4 * 1 },
			{ PixelFormat::R16F, 2 * 1 },
			{ PixelFormat::R16, 2 * 1 },
			{ PixelFormat::RGB16, 2 * 3 },
			{ PixelFormat::RGBA, 4 * 4 },
		};

		return pixelCount * lookup[this->pixelFormat];
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
