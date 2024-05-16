#pragma once

#include <vector>

namespace render
{
	template<class T>
	struct RenderInfoBase
	{
		std::vector<T> data{};

		void add(T&& t) {
			this->data.push_back(std::forward<T>(t));
		}

		int64_t getSize() const {
			return static_cast<int64_t>(this->data.size());
		}
	};
}
