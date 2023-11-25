#pragma once

#include <tepp/enum_array.h>

namespace render
{
	enum class DataType
	{
		f32,
		vec2,
		vec3,
		vec4,
		i32,
		ivec2,
		ivec3,
		ivec4,
		u32,
		uvec2,
		uvec3,
		uvec4,
		mat2,
		mat3,
		mat4,
		mat2x3,
		mat3x2,
		mat2x4,
		mat4x2,
		mat3x4,
		mat4x3,
		coloru32,
		MAX
	};

	static constexpr te::enum_array<DataType, uint32_t> dataTypeByteSize = {
		{ DataType::f32, 4 },
		{ DataType::vec2, 8 },
		{ DataType::vec3, 12 },
		{ DataType::vec4, 16 },
		{ DataType::i32, 4 },
		{ DataType::ivec2, 8 },
		{ DataType::ivec3, 12 },
		{ DataType::ivec4, 16 },
		{ DataType::u32, 4 },
		{ DataType::uvec2, 8 },
		{ DataType::uvec3, 12 },
		{ DataType::uvec4, 16 },
		{ DataType::mat2, 16 },
		{ DataType::mat3, 36 },
		{ DataType::mat4, 64 },
		{ DataType::mat2x3, 24 },
		{ DataType::mat3x2, 24 },
		{ DataType::mat2x4, 32 },
		{ DataType::mat4x2, 32 },
		{ DataType::mat3x4, 48 },
		{ DataType::mat4x3, 48 },
		{ DataType::coloru32, 4 },
	};
}
