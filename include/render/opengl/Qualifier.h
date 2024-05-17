#pragma once

#include <misc/Misc.h>

namespace render::opengl
{
	template<class T>
	struct Qualified
	{
		T data{};
		int64_t qualifier{};

		void clear();
		explicit operator bool() const;

		DEFAULT_COPY(Qualified);
		Qualified(Qualified&& other);
		Qualified& operator=(Qualified&& other);

		Qualified() = default;
		~Qualified() = default;

		bool operator==(Qualified other) const {
			return this->qualifier == other.qualifier && this->data == other.data;
		};
	};

	template<class T>
	inline void Qualified<T>::clear() {
		this->data = {};
		this->qualifier = {};
	}

	template<class T>
	inline Qualified<T>::operator bool() const {
		return this->qualifier != 0;
	}

	template<class T>
	inline Qualified<T>::Qualified(Qualified&& other) {
		(*this) = std::move(other);
	}

	template<class T>
	inline Qualified<T>& Qualified<T>::operator=(Qualified&& other) {
		this->data = other.data;
		this->qualifier = other.qualifier;

		other.clear();

		return *this;
	}
}