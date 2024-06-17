#pragma once

#include <cassert>
#include <span>
#include <vector>

#include <misc/Misc.h>

namespace render
{
	template<class T>
	struct RenderInfoBase
	{
		std::vector<T> data{};

		std::span<T const> getSubSpan(int64_t begin, int64_t end) const {
			assert(begin >= 0);
			assert(end >= 0);
			assert(begin <= isize(this->data));
			assert(end <= isize(this->data));
			assert(begin <= end);

			begin = std::min(isize(this->data), begin);
			end = std::min(isize(this->data), end);
			auto d = std::max(0_i64, end - begin);
			auto everything = std::span(this->data);

			return everything.subspan(begin, d);
		}

		void add(T&& t) {
			this->data.push_back(std::forward<T>(t));
		}

		void append(RenderInfoBase<T> const& other) {
			this->data.insert(this->data.end(), other.data.begin(), other.data.end());
		}

		int64_t getSize() const {
			return static_cast<int64_t>(this->data.size());
		}

		void clear() {
			this->data.clear();
		}
	};
}
