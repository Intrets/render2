#pragma once

#include "render/opengl/OpenglTexture.h"
#include "render/opengl/Qualifier.h"

#include <wrangled_gl/wrangled_gl.h>

#include <optional>

#include <tepp/integers.h>

namespace render::opengl
{
	struct OpenglContext;
	struct Opengl2DTexture;

	struct OpenglPBO;

	template<class T>
	struct OpenglPBOMappedRead
	{
		OpenglPBO* pbo;
		std::span<T const> data;

		NO_COPY(OpenglPBOMappedRead);
		DEFAULT_MOVE(OpenglPBOMappedRead);

		OpenglPBOMappedRead() = delete;
		OpenglPBOMappedRead(OpenglPBO& pbo, std::span<T const> data_);
		~OpenglPBOMappedRead();
	};

	template<class T>
	struct OpenglPBOMappedWrite
	{
		OpenglPBO* pbo;
		std::span<T> data;

		NO_COPY(OpenglPBOMappedWrite);
		DEFAULT_MOVE(OpenglPBOMappedWrite);

		OpenglPBOMappedWrite() = delete;
		OpenglPBOMappedWrite(OpenglPBO& pbo, std::span<T> data_);
		~OpenglPBOMappedWrite();
	};

	struct OpenglPBO
	{
		OpenglContext* openglContext;
		Qualified<GLuint> ID{};
		bool bufferMapped = false;
		std::optional<integer_t> downloadSize{};
		std::optional<TextureFormat> uploadFormat{};

		NO_COPY(OpenglPBO);
		DEFAULT_MOVE(OpenglPBO);

		void bindPack();
		void bindUnpack();
		void unbindPack();
		void unbindUnpack();

		void download(
		    Opengl2DTexture& texture,
		    integer_t level,
		    TextureFormat::PixelFormat pixelFormat
		);

		template<class T>
		std::optional<OpenglPBOMappedRead<T>> getDownload();

		template<class T>
		OpenglPBOMappedWrite<T> getUpload(
		    Opengl2DTexture& texture,
		    integer_t level,
		    TextureFormat::PixelFormat pixelFormat
		);

		void upload(
		    integer_t level,
		    Opengl2DTexture& texture
		);

		template<class T>
		friend struct OpenglPBOMappedRead;

	private:
		void unmapPBO();

	public:
		OpenglPBO(OpenglContext& openglContext);
		~OpenglPBO();
	};

	template<class T>
	inline std::optional<OpenglPBOMappedRead<T>> OpenglPBO::getDownload() {
		if (!this->downloadSize.has_value()) {
			assert(0);
			return std::nullopt;
		}

		if (this->bufferMapped) {
			assert(0);
			return std::nullopt;
		}

		this->bindPack();
		auto data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, static_cast<GLsizeiptr>(this->downloadSize.value()), GL_READ_ONLY);
		this->unbindPack();

		if (data == nullptr) {
			assert(0);
			return std::nullopt;
		}

		auto span = std::span<T const>(reinterpret_cast<T*>(data), this->downloadSize.value() / sizeof(T));

		this->downloadSize.reset();

		return OpenglPBOMappedRead<T>(*this, span);
	}

	template<class T>
	inline OpenglPBOMappedWrite<T> OpenglPBO::getUpload(Opengl2DTexture& texture, integer_t level, TextureFormat::PixelFormat pixelFormat) {
		this->bindUnpack();

		auto dummy = texture.textureFormat;
		dummy.pixelFormat = pixelFormat;

		this->uploadFormat = dummy;

		integer_t bufferSize = dummy.getPixelCount() * sizeof(T);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, nullptr, GL_STREAM_READ);
		auto ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, static_cast<GLsizeiptr>(bufferSize), GL_WRITE_ONLY);

		auto span = std::span<T>(reinterpret_cast<T*>(ptr), texture.textureFormat.getPixelCount());

		this->unbindUnpack();
		return OpenglPBOMappedWrite<T>(*this, span);
	}

	template<class T>
	inline OpenglPBOMappedRead<T>::OpenglPBOMappedRead(OpenglPBO& pbo_, std::span<T const> data_)
	    : pbo(&pbo_),
	      data(data_) {
	}

	template<class T>
	inline OpenglPBOMappedRead<T>::~OpenglPBOMappedRead() {
		this->pbo->unmapPBO();
	}

	template<class T>
	inline OpenglPBOMappedWrite<T>::OpenglPBOMappedWrite(OpenglPBO& pbo_, std::span<T> data_)
	    : pbo(&pbo_),
	      data(data_) {
	}

	template<class T>
	inline OpenglPBOMappedWrite<T>::~OpenglPBOMappedWrite() {
	}
}
