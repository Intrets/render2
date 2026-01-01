#pragma once

#include <cstdlib>
#include <utility>

namespace render
{
	template<class From, class To>
	struct Convert;

	template<class T>
	struct auto_cast
	{
		T const& ref;

		template<class S>
		operator S() {
			return Convert<T, S>::apply(this->ref);
		}

		auto_cast() = delete;
		auto_cast(T const& ref_)
		    : ref(ref_) {
		}
		~auto_cast() = default;
	};

	template<class To, class From>
	To cast(From const& from) {
		return Convert<From, To>::apply(from);
	}

	template<class To, class From>
	To safety_cast(From const& from) {
		if (!std::in_range<To>(from)) {
			std::abort();
		}

		return static_cast<To>(from);
	}

	template<class T>
	struct auto_safety
	{
		T const& ref;

		template<class S>
		operator S() {
			return safety_cast<S, T>(this->ref);
		}

		auto_safety() = delete;
		auto_safety(T const& ref_)
		    : ref(ref_) {
		}
		~auto_safety() = default;
	};
}
