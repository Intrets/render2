#pragma once

#include <memory>

#include "render/opengl/OpenglTexture.h"

#include "tepp/optional_ref.h"

namespace render::opengl
{
	struct ManagedTextureStorage;

	struct ManagedTextureStorageInternal
	{
		Opengl2DTexture data;
		integer_t weakCount = 0;
	};

	struct ManagedTexture
	{
		std::weak_ptr<ManagedTextureStorageInternal> data;

		te::optional_ref<Opengl2DTexture> get();
		void reset();

	private:
		void decrement();
		void increment();

	public:
		ManagedTexture(ManagedTexture const& other);
		ManagedTexture(ManagedTexture&& other);
		ManagedTexture& operator=(ManagedTexture const& other);
		ManagedTexture& operator=(ManagedTexture&& other);

		ManagedTexture();
		ManagedTexture(ManagedTextureStorage& storage);
		~ManagedTexture();
	};

	struct ManagedTextureStorage
	{
		std::shared_ptr<ManagedTextureStorageInternal> data;

		bool unused() const;

		ManagedTexture getManagedTexture();

		static ManagedTextureStorage make(render::opengl::Opengl2DTexture object);
	};
}
