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
}
