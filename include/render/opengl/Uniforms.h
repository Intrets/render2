#pragma once

#include <algorithm>
#include <span>
#include <variant>
#include <vector>

#include <vec/ivec2.h>
#include <vec/mat4.h>
#include <vec/vec2.h>
#include <vec/vec3.h>
#include <vec/vec4.h>

#include <render/opengl/Program.h>

#include <tepp/cstring_view.h>

#include <wrangled_gl/wrangled_gl.h>

#include <game/misc/Convert.h>

namespace render::opengl
{
	template<class T>
	struct SetUniform;

	struct Opengl2DTexture;
	struct Opengl2DArrayTexture;
	struct OpenglBufferTexture;

	template<>
	struct SetUniform<float>
	{
		static void apply(GLuint location, std::span<float const> values) {
			glUniform1fv(location, game::auto_safety(values.size()), values.data());
		}
	};

	template<>
	struct SetUniform<vec::ivec2>
	{
		static void apply(GLuint location, std::span<vec::ivec2 const> values) {
			glUniform2iv(location, game::auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::vec2>
	{
		static void apply(GLuint location, std::span<vec::vec2 const> values) {
			glUniform2fv(location, game::auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::vec3>
	{
		static void apply(GLuint location, std::span<vec::vec3 const> values) {
			glUniform3fv(location, game::auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::vec4>
	{
		static void apply(GLuint location, std::span<vec::vec4 const> values) {
			glUniform4fv(location, game::auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::mat4>
	{
		static void apply(GLuint location, std::span<vec::mat4 const> values) {
			glUniformMatrix4fv(location, game::auto_safety(values.size()), GL_FALSE, values[0].data());
		}
	};

	template<class T>
	struct Uniform
	{
		std::optional<std::variant<T, std::vector<T>>> current{};
		Program* program{};
		GLuint location{};

		Uniform() = default;
		void initialize(te::cstring_view name, Program& program_) {
			this->program = &program_;
			this->location = glGetUniformLocation(this->program->ID.data, name.getData());
		}
		DEFAULT_COPY_MOVE(Uniform);
		~Uniform() = default;

		void set(T const& value) {
			assert(this->program);

			if (this->current.has_value()) {
				assert(std::holds_alternative<T>(this->current.value()));

				if (auto currentValue = std::get_if<T>(&this->current.value())) {
					if (*currentValue == value) {
						return;
					}
				}
			}

			this->current = value;

			this->program->use();

			SetUniform<T>::apply(this->location, std::span<T const>(&value, 1));
		}

		void set(std::span<T> values) {
			assert(this->program);

			if (this->current.has_value()) {
				assert(std::holds_alternative<std::vector<T>>(this->current.value()));

				if (auto currentValue = std::get_if<T>(&this->current.value())) {
					if (std::ranges::equal(*currentValue, values)) {
						return;
					}
				}
			}

			std::ranges::copy(values, this->current);

			this->program->use();

			SetUniform<T>::apply(this->location, values);
		}
	};

	struct OpenglSampler2D
	{
		int32_t unit{};
		Program* program{};
		GLuint location{};

		OpenglSampler2D() = default;
		void initialize(te::cstring_view name, Program& program);
		NO_COPY_MOVE(OpenglSampler2D);
		~OpenglSampler2D() = default;

		void set(Opengl2DTexture const& texture);
	};

	struct OpenglSampler3D
	{
		int32_t unit{};
		Program* program{};
		GLuint location{};

		OpenglSampler3D() = default;
		void initialize(te::cstring_view name, Program& program);
		NO_COPY_MOVE(OpenglSampler3D);
		~OpenglSampler3D() = default;

		void set(Opengl2DArrayTexture const& texture);
	};

	struct OpenglSamplerBufferTexture
	{
		int32_t unit{};
		Program* program{};
		GLuint location{};

		OpenglSamplerBufferTexture() = default;
		void initialize(te::cstring_view name, Program& program);
		NO_COPY_MOVE(OpenglSamplerBufferTexture);
		~OpenglSamplerBufferTexture() = default;

		void set(OpenglBufferTexture const& texture);
	};
}