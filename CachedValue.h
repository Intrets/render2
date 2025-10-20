#pragma once

#include <concepts>

#include <tepp/angle.h>
#include <tepp/in_range.h>
#include <tepp/integers.h>

#include "render/RenderInfoBase.h"

#include <misc/Misc.h>

namespace render
{
	namespace detail
	{
		template<auto ptr>
		struct AccessesValue;

		template<class Info, class ValueType, ValueType Info::* ptr>
		struct AccessesValue<ptr>
		{
			using type = ValueType;
			using base = Info;
		};
	}

	template<auto ptr, class StorageType = typename detail::template AccessesValue<ptr>::type, class I = typename detail::template AccessesValue<ptr>::base>
	struct CachedValue
	{
		using ValueType = typename detail::template AccessesValue<ptr>::type;
		using Info = typename detail::template AccessesValue<ptr>::base;

		StorageType value{};
		qualifier_t qualifier{ -1 };
		render::RenderInfoBase<Info>* info = nullptr;

		integer_t index = 0;

		StorageType& get() {
			return this->value;
		}

		StorageType const& get() const {
			return this->value;
		}

		operator StorageType() {
			return this->value;
		}

		operator StorageType const() const {
			return this->value;
		}

		StorageType* operator->() {
			return &this->value;
		}

		StorageType const* operator->() const {
			return &this->value;
		}

		CachedValue() = default;
		~CachedValue() = default;

		template<class... Args>
		CachedValue(Args&&... args)
		    : value(std::forward<Args>(args)...) {
		}

		CachedValue& operator=(StorageType const& v) {
			this->value = v;

			return *this;
		}

		CachedValue& operator=(StorageType&& v) {
			this->value = std::move(v);

			return *this;
		}

		DEFAULT_COPY_MOVE(CachedValue);

		void send(qualifier_t qualifier, StorageType s) {
			if (this->qualifier == qualifier && this->value != s && te::in_range(this->index, this->info->getData())) {
				if constexpr (std::same_as<te::angle<float>, StorageType>) {
					this->info->get(this->index).*ptr = s.get();
				}
				else {
					this->info->get(this->index).*ptr = s;
				}
			}

			this->value = s;
		}

		void clear() {
		}
	};

	template<auto ptr, class StorageType = typename detail::template AccessesValue<ptr>::type, class I = typename detail::template AccessesValue<ptr>::base>
	struct CachedValues
	{
		using ValueType = typename detail::template AccessesValue<ptr>::type;
		using Info = typename detail::template AccessesValue<ptr>::base;

		StorageType value{};

		qualifier_t qualifier{ -1 };
		render::RenderInfoBase<Info>* info = nullptr;

		std::vector<integer_t> indices{};

		StorageType& get() {
			return this->value;
		}

		StorageType const& get() const {
			return this->value;
		}

		operator StorageType() {
			return this->value;
		}

		operator StorageType const() const {
			return this->value;
		}

		StorageType* operator->() {
			return &this->value;
		}

		StorageType const* operator->() const {
			return &this->value;
		}

		void clear(StorageType const& v) {
			this->indices.clear();
			this->info = nullptr;
			this->value = v;
		}

		CachedValues() = default;
		~CachedValues() = default;

		template<class... Args>
		CachedValues(Args&&... args)
		    : value(std::forward<Args>(args)...) {
		}

		CachedValues& operator=(StorageType const& v) {
			this->value = v;

			return *this;
		}

		CachedValues& operator=(StorageType&& v) {
			this->value = std::move(v);

			return *this;
		}

		DEFAULT_COPY_MOVE(CachedValues);

		bool isValid() const {
			return true;
		}

		void send(qualifier_t qualifier, StorageType s) {
			if (this->qualifier == qualifier && this->value != s && this->isValid()) {
				if constexpr (std::same_as<te::angle<float>, StorageType>) {
					for (auto index : this->indices) {
						this->info->get(index).*ptr = s.get();
					}
				}
				else {
					for (auto index : this->indices) {
						this->info->get(index).*ptr = s;
					}
				}
			}

			this->value = s;
		}
	};

	template<class Info>
	struct CachedValueInitializer
	{
		render::RenderInfoBase<Info>& info;
		qualifier_t qualifier;
		integer_t index;

		template<auto ptr, class T>
		CachedValueInitializer<Info>& init(CachedValue<ptr, T>& value) {
			value.info = &this->info;
			value.qualifier = this->qualifier;
			value.index = this->index;

			return *this;
		}

		template<auto ptr, class T>
		CachedValueInitializer<Info>& init(CachedValues<ptr, T>& value) {
			tassert(value.info == nullptr || value.info == &this->info);
			value.info = &this->info;
			value.qualifier = this->qualifier;
			value.indices.push_back(this->index);

			return *this;
		}
	};
}
