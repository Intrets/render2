#pragma once

#include <vector>

#include <misc/Misc.h>

#include <tepp/assert.h>
#include <tepp/span.h>

namespace render
{
	template<class T>
	struct RenderInfoBase
	{
	private:
		std::vector<T> data{};

	public:
		te::span<T const> getData() const {
			return this->data;
		}

		std::vector<T>& getVector() {
			return this->data;
		}

		std::vector<T> const& getVector() const {
			return this->data;
		}

		T& get(integer_t i) {
			return this->data[i];
		}

		T const& get(integer_t i) const {
			return this->data[i];
		}

		te::span<T> getSubSpan(integer_t begin, integer_t end) {
			tassert(begin >= 0);
			tassert(end >= 0);
			tassert(begin <= isize(this->data));
			tassert(end <= isize(this->data));
			tassert(begin <= end);

			begin = std::min(isize(this->data), begin);
			end = std::min(isize(this->data), end);
			auto d = std::max(0_i, end - begin);
			auto everything = te::span(this->data);

			return everything.subspan(begin, d);
		}

		te::span<T const> getSubSpan(integer_t begin, integer_t end) const {
			tassert(begin >= 0);
			tassert(end >= 0);
			tassert(begin <= isize(this->data));
			tassert(end <= isize(this->data));
			tassert(begin <= end);

			begin = std::min(isize(this->data), begin);
			end = std::min(isize(this->data), end);
			auto d = std::max(0_i, end - begin);
			auto everything = te::span(this->data);

			return everything.subspan(begin, d);
		}

		void add(T&& t) {
			this->data.push_back(std::forward<T>(t));
		}

		void add(T const& t) {
			this->data.push_back(t);
		}

		T& back() {
			return this->data.back();
		}

		T& front() {
			return this->data.front();
		}

		void pop_back() {
			this->data.pop_back();
		}

		void append(RenderInfoBase<T> const& other) {
			this->data.insert(this->data.end(), other.data.begin(), other.data.end());
		}

		integer_t size() const {
			return isize(this->data);
		}

		bool empty() const {
			return this->data.empty();
		}

		virtual void clear() {
			this->data.clear();
		}

		RenderInfoBase() = default;
		virtual ~RenderInfoBase() = default;
	};
}
