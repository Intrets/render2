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

		DEFAULT_COPY_MOVE(Qualified);

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
}