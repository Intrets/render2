#include "render/Color.h"

namespace render
{
	Color Color::fromVec4(glm::vec4 vec) {
		return rgba(
		    static_cast<uint32_t>(vec.r * 255.0f),
		    static_cast<uint32_t>(vec.g * 255.0f),
		    static_cast<uint32_t>(vec.b * 255.0f),
		    static_cast<uint32_t>(vec.a * 255.0f)
		);
	}

	glm::vec3 toLinear(glm::vec3 v) {
		return {
			::toLinear(v.x),
			::toLinear(v.y),
			::toLinear(v.z),
		};
	}

	Color alpha(Color c, float val) {
		tassert(0.0 <= val && val <= 1.0);

		return alpha(c, std::clamp(static_cast<integer_t>(255 * val), 0_i, 255_i));
	}

	Color alpha(Color c, integer_t val) {
		tassert(0 <= val && val < 256);

		return { c.c & (0x00FFFFFF | (static_cast<uint32_t>(val) << 24)) };
	}

	static constexpr std::array<Color, 5> nice_colors = {
		nice_blue,
		nice_darkblue,
		nice_green,
		nice_red,
		orange,
	};

	static constexpr std::array<Color, 30> reds_and_purples = {
		converter(rgb(205, 92, 92)),
		converter(rgb(178, 34, 34)),
		converter(rgb(165, 42, 42)),
		converter(rgb(233, 150, 122)),
		converter(rgb(250, 128, 114)),
		converter(rgb(255, 160, 122)),
		converter(rgb(255, 127, 80)),
		converter(rgb(240, 128, 128)),
		converter(rgb(255, 99, 71)),
		converter(rgb(255, 69, 0)),
		converter(rgb(255, 0, 0)),
		converter(rgb(255, 105, 180)),
		converter(rgb(255, 20, 147)),
		converter(rgb(255, 192, 203)),
		converter(rgb(255, 182, 193)),
		converter(rgb(219, 112, 147)),
		converter(rgb(176, 48, 96)),
		converter(rgb(199, 21, 133)),
		converter(rgb(208, 32, 144)),
		converter(rgb(255, 0, 255)),
		converter(rgb(238, 130, 238)),
		converter(rgb(221, 160, 221)),
		converter(rgb(218, 112, 214)),
		converter(rgb(186, 85, 211)),
		converter(rgb(153, 50, 204)),
		converter(rgb(148, 0, 211)),
		converter(rgb(138, 43, 226)),
		converter(rgb(160, 32, 240)),
		converter(rgb(147, 112, 219)),
		converter(rgb(216, 191, 216)),
	};

	static constexpr std::array<Color, 24> twocolors{
		converter(rgb(192, 50, 50)),
		converter(rgb(192, 86, 50)),
		converter(rgb(192, 121, 50)),
		converter(rgb(192, 156, 50)),
		converter(rgb(192, 192, 50)),
		converter(rgb(156, 192, 50)),
		converter(rgb(121, 192, 50)),
		converter(rgb(86, 192, 50)),
		converter(rgb(50, 192, 50)),
		converter(rgb(50, 192, 86)),
		converter(rgb(50, 192, 121)),
		converter(rgb(50, 192, 156)),
		converter(rgb(50, 192, 192)),
		converter(rgb(50, 156, 192)),
		converter(rgb(50, 121, 192)),
		converter(rgb(50, 86, 192)),
		converter(rgb(50, 50, 192)),
		converter(rgb(86, 50, 192)),
		converter(rgb(121, 50, 192)),
		converter(rgb(156, 50, 192)),
		converter(rgb(192, 50, 192)),
		converter(rgb(192, 50, 156)),
		converter(rgb(192, 50, 121)),
		converter(rgb(192, 50, 86)),
	};

	twocolor::operator Color() {
		return twocolors[this->index];
	}

	twocolor twocolor::operator-(integer_t i) {
		return { (this->index - i) % isize(twocolors) };
	}

	twocolor twocolor::operator+(integer_t i) {
		return { (i + this->index) % isize(twocolors) };
	}

	twocolor twocolor::blue() {
		return { 16 };
	}

	twocolor twocolor::green() {
		return { 8 };
	}

	twocolor twocolor::red() {
		return { 0 };
	}

	Color getNiceColor(integer_t i) {
		return nice_colors[std::clamp(i, 0_i, isize(nice_colors) - 1)];
	}

	Color getRandomNiceColor(std::minstd_rand& rng) {
		auto i = std::uniform_int_distribution<integer_t>(0, nice_colors.size() - 1)(rng);
		return getNiceColor(i);
	}

	glm::vec4 Color::toVec4() const {
		return {
			this->redNormalized(),
			this->greenNormalized(),
			this->blueNormalized(),
			this->alphaNormalized(),
		};
	}

	Color Color::mix(Color other, float factor) const {
		return fromVec4(glm::mix(this->toVec4(), other.toVec4(), factor));
	}

	void Color::setAlpha(float v) {
		this->c &= 0xFFFFFF;
		this->c |= (static_cast<uint8_t>(v * 255) << 24);
	}

	float Color::redNormalized() const {
		return (0xFF & this->c) / 255.0f;
	}

	float Color::greenNormalized() const {
		return (0xFF & (this->c >> 8)) / 255.0f;
	}

	float Color::blueNormalized() const {
		return (0xFF & (this->c >> 16)) / 255.0f;
	}

	float Color::alphaNormalized() const {
		return ((this->c >> 24) & 0xFF) / 255.0f;
	}

	integer_t twocolor::size() {
		return isize(twocolors);
	}
}
