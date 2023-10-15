#include "render/opengl/OpenglTexture.h"

#include <tepp/enum_array.h>

#include "render/opengl/OpenglContext.h"

namespace render::opengl
{
	GLint TextureFormat::getInternalFormat() const {
		constexpr te::enum_array<PixelFormat, GLint> lookup{
			{ PixelFormat::RGBA, GL_RGBA },
			{ PixelFormat::RGBA16, GL_RGBA16 },
			{ PixelFormat::RGBA32, GL_RGBA32F },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataFormat() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::RGBA, GL_RGBA },
			{ PixelFormat::RGBA16, GL_RGBA },
			{ PixelFormat::RGBA32, GL_RGBA },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getPixelDataType() const {
		constexpr te::enum_array<PixelFormat, GLenum> lookup{
			{ PixelFormat::RGBA, GL_UNSIGNED_BYTE },
			{ PixelFormat::RGBA16, GL_HALF_FLOAT },
			{ PixelFormat::RGBA32, GL_FLOAT },
		};

		return lookup[this->pixelFormat];
	}

	GLenum TextureFormat::getMinFilter() const {
		constexpr te::enum_array<Filtering, GLenum> lookup{
			{ Filtering::LINEAR, GL_LINEAR_MIPMAP_LINEAR },
			{ Filtering::NEAREST, GL_NEAREST_MIPMAP_NEAREST },
		};

		return lookup[this->filtering];
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

	int64_t TextureFormat::getByteSize() const {
		int64_t pixelCount = static_cast<int64_t>(this->size.x()) * this->size.y();

		constexpr te::enum_array<PixelFormat, int64_t> lookup{
			{ PixelFormat::RGBA, 4 * 4 },
			{ PixelFormat::RGBA16, 16 * 4 },
			{ PixelFormat::RGBA32, 32 * 4 },
		};

		return pixelCount * lookup[this->pixelFormat];
	}

	void Opengl2DTexture::bind() {
		this->openglContext.bind(*this);
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

	std::optional<Opengl2DTexture> Opengl2DTexture::make(OpenglContext& openglContext, TextureFormat const& textureFormat, std::optional<std::span<std::byte>> data) {
		int32_t maxSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		if (textureFormat.size > maxSize) {
			openglContext.logError("Tried to make texture with size {} {}, maximum size supported is {}.\n", textureFormat.size.x(), textureFormat.size.y(), maxSize);
			return std::nullopt;
		}

		auto result = Opengl2DTexture(openglContext);
		glGenTextures(1, &this->ID.data);

		this->bind();

		void* ptr = nullptr;

		if (data.has_value() && !data->empty()) {
			if (textureFormat.getByteSize() != data->size()) {
				openglContext.logError("Mismatched byte size when trying to load texture. Wanted {}, have {}.\n", textureFormat.getByteSize(), data->size());

				return std::nullopt;
			}

			ptr = data->data();
		}

		glTexImage2D(
		    GL_TEXTURE_2D,
		    0,
		    textureFormat.getInternalFormat(),
		    textureFormat.size.x(),
		    textureFormat.size.y(),
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
}
