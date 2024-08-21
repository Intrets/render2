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

	struct OpenglPBO
	{
		OpenglContext* openglContext;
		Qualified<GLuint> ID{};
		bool bufferMapped = false;
		std::optional<integer_t> downloadSize{};

		NO_COPY(OpenglPBO);
		DEFAULT_MOVE(OpenglPBO);

		void bind();

		void download(
		    Opengl2DTexture& texture,
		    integer_t level,
		    TextureFormat::PixelFormat pixelFormat
		);

		template<class T>
		std::optional<OpenglPBOMappedRead<T>> getDownload();

		template<class T>
		friend struct OpenglPBOMappedRead;

	private:
		void unmapDownload();

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

		this->bind();
		auto data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

		if (data == nullptr) {
			assert(0);
			return std::nullopt;
		}

		auto span = std::span<T const>(reinterpret_cast<T*>(data), this->downloadSize.value() / sizeof(T));

		this->downloadSize.reset();

		return OpenglPBOMappedRead<T>(*this, span);
	}

	template<class T>
	inline OpenglPBOMappedRead<T>::OpenglPBOMappedRead(OpenglPBO& pbo_, std::span<T const> data_)
	    : pbo(&pbo_),
	      data(data_) {
	}

	template<class T>
	inline OpenglPBOMappedRead<T>::~OpenglPBOMappedRead() {
		this->pbo->unmapDownload();
	}
}
