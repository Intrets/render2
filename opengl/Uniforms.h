#pragma once

#include <algorithm>
#include <span>
#include <variant>
#include <vector>

#include <wglm/mat4x4.hpp>
#include <wglm/vec2.hpp>
#include <wglm/vec3.hpp>
#include <wglm/vec4.hpp>

#include <render/opengl/OpenglContext.h>
#include <render/opengl/Program.h>

#include <tepp/cstring_view.h>
#include <tepp/variant.h>

#include <wrangled_gl/wrangled_gl.h>

#include "render/Convert.h"

namespace render::opengl
{
	template<class T>
	struct SetUniform;

	struct Opengl2DTexture;
	struct Opengl2DArrayTexture;
	struct OpenglBufferTexture;

	struct UniformBase
	{
		Program* program{};
		GLuint location{};
		std::string name{};

		virtual te::cstring_view getValueType() = 0;
		te::cstring_view getName() const;
	};

	template<>
	struct SetUniform<float>
	{
		static te::cstring_view name() {
			return "float";
		}

		static void apply(GLuint location, std::span<float const> values) {
			glUniform1fv(location, auto_safety(values.size()), values.data());
		}
	};

	template<>
	struct SetUniform<glm::ivec2>
	{
		static te::cstring_view name() {
			return "ivec2";
		}

		static void apply(GLuint location, std::span<glm::ivec2 const> values) {
			glUniform2iv(location, auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<glm::vec2>
	{
		static te::cstring_view name() {
			return "vec2";
		}

		static void apply(GLuint location, std::span<glm::vec2 const> values) {
			glUniform2fv(location, auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<glm::vec3>
	{
		static te::cstring_view name() {
			return "vec3";
		}

		static void apply(GLuint location, std::span<glm::vec3 const> values) {
			glUniform3fv(location, auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<glm::vec4>
	{
		static te::cstring_view name() {
			return "vec4";
		}

		static void apply(GLuint location, std::span<glm::vec4 const> values) {
			glUniform4fv(location, auto_safety(values.size()), &values[0][0]);
		}
	};

	template<>
	struct SetUniform<glm::mat4>
	{
		static te::cstring_view name() {
			return "mat4";
		}

		static void apply(GLuint location, std::span<glm::mat4 const> values) {
			glUniformMatrix4fv(location, auto_safety(values.size()), GL_FALSE, &values[0][0][0]);
		}
	};

	template<>
	struct SetUniform<int32_t>
	{
		static te::cstring_view name() {
			return "int32";
		}

		static void apply(GLuint location, std::span<int32_t const> values) {
			glUniform1iv(location, auto_safety(values.size()), &values[0]);
		}
	};

	template<class T>
	struct Uniform : UniformBase
	{
		std::optional<std::variant<T, std::vector<T>>> current{};

		te::cstring_view getValueType() override {
			return SetUniform<T>::name();
		}

		Uniform() = default;
		void initialize(te::cstring_view name_, Program& program_) {
			auto refresh = this->program != nullptr;

			this->program = &program_;
			this->program->use();
			this->location = glGetUniformLocation(this->program->ID.data, name_.getData());
			if (refresh) {
				tassert(this->name == name_);
				if (this->current.has_value()) {
					te::visit(this->current.value(), [&](auto& v) {
						this->set(v, true);
					});
				}
			}
			else {
				this->name = name_;
				this->program->registerUniform(*this);
				this->current.reset();
			}
		}
		DEFAULT_COPY_MOVE(Uniform);
		~Uniform() = default;

		void set(std::span<T const> values, bool force = false) {
			tassert(this->program);

			if (!force && this->current.has_value()) {
				tassert(std::holds_alternative<std::vector<T>>(this->current.value()));

				auto done = te::visit(
				    this->current.value(),
				    [&](std::vector<T>& vec) {
					    if (std::ranges::equal(values, vec)) {
						    return true;
					    }

					    std::ranges::copy(values, vec.begin());

					    return false;
				    },
				    [](auto&) {
					    return false;
				    }
				);

				if (done) {
					return;
				}
			}

			this->program->use();

			this->program->openglContext.tallyUniformBytesTransferred(values.size_bytes());
			SetUniform<T>::apply(this->location, values);
			auto c = std::vector<T>();
			c.resize(values.size());
			std::ranges::copy(values, c.begin());
			this->current = std::move(c);
		}

		void set(T const& value, bool force = false) {
			tassert(this->program);

			if (!force && this->current.has_value()) {
				tassert(std::holds_alternative<T>(this->current.value()));

				if (auto currentValue = std::get_if<T>(&this->current.value())) {
					if (*currentValue == value) {
						return;
					}
				}
			}

			this->current = value;

			this->program->use();

			this->program->openglContext.tallyUniformBytesTransferred(sizeof(T));
			SetUniform<T>::apply(this->location, std::span<T const>(&value, 1));
		}
	};

	struct OpenglSampler2D : UniformBase
	{
		std::vector<GLint> units{};

		te::cstring_view getValueType() override;

		OpenglSampler2D() = default;
		void initialize(te::cstring_view name, Program& program);
		void initialize(te::cstring_view name, integer_t count, Program& program);
		DEFAULT_MOVE(OpenglSampler2D);
		NO_COPY(OpenglSampler2D);
		~OpenglSampler2D() = default;

		void set(Opengl2DTexture const& texture);
		void set(Opengl2DTexture const& texture, integer_t index);
		void set(Qualified<GLuint> ID, integer_t index);
	};

	struct OpenglSampler3D : UniformBase
	{
		int32_t unit{};

		te::cstring_view getValueType() override;

		OpenglSampler3D() = default;
		void initialize(te::cstring_view name, Program& program);
		DEFAULT_MOVE(OpenglSampler3D);
		NO_COPY(OpenglSampler3D);
		~OpenglSampler3D() = default;

		void set(Opengl2DArrayTexture const& texture);
	};

	struct OpenglSamplerBufferTexture : UniformBase
	{
		int32_t unit{};

		te::cstring_view getValueType() override;

		OpenglSamplerBufferTexture() = default;
		void initialize(te::cstring_view name, Program& program);
		DEFAULT_MOVE(OpenglSamplerBufferTexture);
		NO_COPY(OpenglSamplerBufferTexture);
		~OpenglSamplerBufferTexture() = default;

		void set(OpenglBufferTexture const& texture);
	};
}