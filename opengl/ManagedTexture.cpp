#include "render/opengl/ManagedTexture.h"
#include "tepp/nullopt.h"
#include "tepp/optional.h"
#include <memory>

namespace render::opengl
{
	te::optional_ref<Opengl2DTexture> ManagedTexture::get() {
		auto ptr = this->data.lock();

		if (ptr != nullptr) {
			return ptr->data;
		}
		else {
			return te::nullopt;
		}
	}

	ManagedTexture ManagedTextureStorage::getManagedTexture() {
		return ManagedTexture(*this);
	}

	bool ManagedTextureStorage::unused() const {
		return this->data.use_count() == 1 && this->data->weakCount == 0;
	}

	ManagedTexture& ManagedTexture::operator=(ManagedTexture const& other) {
		this->decrement();
		this->data = other.data;
		this->increment();

		return *this;
	}

	ManagedTexture& ManagedTexture::operator=(ManagedTexture&& other) {
		this->decrement();
		this->data = std::move(other.data);

		return *this;
	}

	ManagedTexture::ManagedTexture(ManagedTexture&& other) {
		this->data = std::move(other.data);
	}

	ManagedTexture::ManagedTexture(ManagedTexture const& other) {
		this->data = other.data;
		this->increment();
	}

	void ManagedTexture::increment() {
		if (auto ptr = this->data.lock()) {
			ptr->weakCount++;
		}
	}

	void ManagedTexture::decrement() {
		if (auto ptr = this->data.lock()) {
			ptr->weakCount--;
		}
	}

	ManagedTexture::~ManagedTexture() {
		this->decrement();
	}

	ManagedTexture::ManagedTexture(ManagedTextureStorage& storage) {
		this->data = storage.data;
		this->increment();
	}

	ManagedTexture::ManagedTexture() {
	}

	ManagedTextureStorage ManagedTextureStorage::make(render::opengl::Opengl2DTexture object) {
		auto result = ManagedTextureStorage();
		result.data = std::make_shared<ManagedTextureStorageInternal>(std::move(object));
		return result;
	}

	void ManagedTexture::reset() {
		this->decrement();
		this->data.reset();
	}
}
