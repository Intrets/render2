#pragma once

#include "SRGBConversion.h"

#include <misc/Misc.h>

#include <wglm/glm.hpp>

#include <tepp/assert.h>
#include <tepp/integers.h>

#include <bit>
#include <random>

namespace render
{
	glm::vec3 toLinear(glm::vec3 v);

	struct Color
	{
		uint32_t c;

		float redNormalized() const;
		float greenNormalized() const;
		float blueNormalized() const;
		float alphaNormalized() const;

		void setAlpha(float v);

		bool operator==(Color other) {
			return this->c == other.c;
		}

		bool operator!=(Color other) {
			return this->c != other.c;
		}

		glm::vec4 toVec4() const;

		static Color fromVec4(glm::vec4 vec);

		Color mix(Color other, float factor) const;
	};

	template<class T>
	constexpr T id(T t) {
		return t;
	};

	constexpr Color toLinear(Color other) {
		struct H
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

		H h = std::bit_cast<H>(other);

		h.r = ::toLinear(h.r);
		h.g = ::toLinear(h.g);
		h.b = ::toLinear(h.b);
		h.a = h.a;

		return std::bit_cast<Color>(h);
	}

	using converterFunctionType = Color (*)(Color);
#ifdef ANDROID
	constexpr converterFunctionType converter = id;
#else
	constexpr converterFunctionType converter = toLinear;
#endif

	constexpr Color rgb(uint32_t r, uint32_t g, uint32_t b) {
		return { r | (g << 8) | (b << 16) | 0xFF000000 };
	}

	constexpr Color crgb(uint32_t r, uint32_t g, uint32_t b) {
		return converter(rgb(r, g, b));
	}

	constexpr Color rgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		return { r | (g << 8) | (b << 16) | (a << 24) };
	}

	constexpr Color crgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		return converter(rgba(r, g, b, a));
	}

	static constexpr Color nothing = { 0x0 };
	static constexpr Color black = converter({ 0xFF000000 });
	static constexpr Color red = converter({ 0xFF0000FF });
	static constexpr Color green = converter({ 0xFF00FF00 });
	static constexpr Color blue = converter({ 0xFFFF0000 });
	static constexpr Color magenta = converter({ 0xFFFF00FF });
	static constexpr Color yellow = converter({ 0xFF00FFFF });
	static constexpr Color cyan = converter({ 0xFFFFFF00 });
	static constexpr Color white = converter({ 0xFFFFFFFF });
	static constexpr Color dlue = converter({ 0xFF261F00 });
	static constexpr Color dark_sky = converter({ 0xFFBA9264 });
	static constexpr Color lighter_dlue = converter({ 0xFF2F2708 });
	static constexpr Color nice_blue = converter({ 0xFFFFAC00 });
	static constexpr Color nice_light_blue = converter({ 0xFFFFD783 });
	static constexpr Color nice_darkblue = converter({ 0xFFFF6900 });
	static constexpr Color nice_green = converter({ 0xFF00CA1D });
	static constexpr Color nice_red = converter({ 0xFF2525E5 });
	static constexpr Color orange = converter({ 0xFF00B2FF });
	static constexpr Color dark_orange = converter({ 0xFF005BFF });
	static constexpr Color light_gray = converter({ 0xFF404040 });
	static constexpr Color gray = converter({ 0xFF808080 });
	static constexpr Color dark_gray = converter({ 0xFFBFBFBF });
	static constexpr Color darker_gray = converter({ 0xFF555555 });
	static constexpr Color gray_red = converter({ 0xFF404080 });
	static constexpr Color gray_green = converter({ 0xFF408040 });
	static constexpr Color gray_blue = converter({ 0xFF804040 });
	static constexpr Color selection_color = converter(rgb(100, 255, 255));
	static constexpr Color shadow_color = converter(rgba(0, 0, 0, 150));
	static constexpr Color background_color = converter(rgba(0, 0, 0, 50));
	static constexpr Color invert_shadow_color = converter(rgba(255, 255, 255, 20));
	static constexpr Color transparent_dark = converter(rgba(0, 0, 0, 230));

	static constexpr Color dial_border_gray = converter(rgb(188, 188, 188));
	static constexpr Color highlight_color = rgb(255, 255, 255);

	static constexpr Color gray_translucent = converter(rgba(0, 0, 0, 100));

	static constexpr Color max_signal_color = rgb(0, 115, 247);

	Color getNiceColor(integer_t i);
	Color getRandomNiceColor(std::minstd_rand& rng);

	struct twocolor
	{
		static twocolor red();
		static twocolor green();
		static twocolor blue();
		static integer_t size();

		integer_t index = 0;

		twocolor operator+(integer_t i);
		twocolor operator-(integer_t i);

		operator Color();
	};

	template<class T>
	Color uniqueColor(T val) {
		return { static_cast<uint32_t>(std::hash<T>{}(val)) | 0xFF000000 };
	}

	Color alpha(Color c, integer_t val);

	Color alpha(Color c, float val);
}
