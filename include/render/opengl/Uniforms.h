#pragma once

#include <algorithm>
#include <span>
#include <variant>
#include <vector>

#include <vec/mat4.h>
#include <vec/vec2.h>
#include <vec/vec3.h>
#include <vec/vec4.h>

#include <render/opengl/Program.h>

#include <tepp/cstring_view.h>

#include <wrangled_gl/wrangled_gl.h>

namespace render::opengl
{
	template<class T>
	struct SetUniform;

	template<>
	struct SetUniform<vec::vec2>
	{
		static void apply(GLuint location, std::span<vec::vec2 const> values) {
			glUniform2fv(location, values.size(), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::vec3>
	{
		static void apply(GLuint location, std::span<vec::vec3 const> values) {
			glUniform3fv(location, values.size(), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::vec4>
	{
		static void apply(GLuint location, std::span<vec::vec4 const> values) {
			glUniform4fv(location, values.size(), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<vec::mat4>
	{
		static void apply(GLuint location, std::span<vec::mat4 const> values) {
			glUniformMatrix4fv(location, values.size(), GL_FALSE, values[0].data());
		}
	};

	template<class T>
	struct Uniform
	{
		std::optional<std::variant<T, std::vector<T>>> current{};
		Program* program{};
		GLuint location{};

		Uniform() = default;
		void initialize(te::cstring_view name, Program& program) {
			this->program = &program;
			this->location = glGetUniformLocation(program.ID.data, name.getData());
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
}