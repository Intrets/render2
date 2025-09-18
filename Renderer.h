#pragma once

#include "render/DataType.h"

#include <string_view>

namespace render
{
	struct Renderer
	{
		virtual void addUniform(std::string_view name, DataType uniformDataType, uint32_t count = 1) = 0;

		virtual void updateUniform(std::string_view name, void* data) = 0;
	};
}
