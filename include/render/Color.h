#pragma once

namespace render
{
#define COLORS_LIST(X) \
	X(white, 255, 255, 255, 255) \
	X(red, 255, 0, 0, 255) \
	X(green, 0, 255, 0, 255)\
	X(blue, 0, 0, 255, 255)

	struct Color
	{
		uint32_t data{};

		static constexpr Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			auto constexpr convert = [](uint8_t i) { return static_cast<uint32_t>(i); };
			return { .data = convert(r) | (convert(g) << 8) | (convert(b) << 16) | (convert(a) << 24) };
		}
	};

	namespace Colors
	{
#define DEFINE_COLOR(name, red, green, blue, alpha) constexpr Color name = Color::rgba(red, green, blue, alpha);

		COLORS_LIST(DEFINE_COLOR);
	}
}
