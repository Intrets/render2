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

		void append(RenderInfoBase<T> const& other) {
			this->data.insert(this->data.begin(), other.data.begin(), other.data.end());
		}

		int64_t getSize() const {
			return static_cast<int64_t>(this->data.size());
		}
	};
}
