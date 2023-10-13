#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <vector>

#include <tepp/box.h>

#include <wrangled_gl/wrangled_gl.h>

#include "render/opengl/Qualifier.h"

namespace render::opengl
{
	struct OpenglContext;

	struct Data
	{
		virtual std::span<char const> get() const = 0;
	};

	struct ReferenceData : Data
	{
		std::span<char const> data{};

		virtual std::span<char const> get() const override;
	};

	struct DynamicData : Data
	{
		std::vector<char> data{};

		virtual std::span<char const> get() const override;
	};

	using DataSourceType = std::unique_ptr<Data>;

	struct DataSource
	{
		virtual std::optional<DataSourceType> data() const = 0;
		virtual std::unique_ptr<DataSource> copy() const = 0;
	};

	struct FileSource : DataSource
	{
		std::filesystem::path path{};

		FileSource() = default;
		FileSource(std::filesystem::path path_);

		std::optional<DataSourceType> data() const override;
		std::unique_ptr<DataSource> copy() const override;
	};

	struct StaticSource : DataSource
	{
		ReferenceData staticData{};

		std::optional<DataSourceType> data() const override;
		std::unique_ptr<DataSource> copy() const override;
	};

	struct DynamicSource : DataSource
	{
		DynamicData dynamicData{};

		std::optional<DataSourceType> data() const override;
		std::unique_ptr<DataSource> copy() const override;
	};

	struct Shader
	{
		GLint ID = 0;

		Shader() = default;

		Shader(Shader const&) = delete;
		Shader& operator=(Shader const&) = delete;

		Shader(Shader&& other);
		Shader& operator=(Shader&& other);

		~Shader();

		static Shader makeVertexShader();
		static Shader makeFragmentShader();
	};

	struct Program
	{
		std::unique_ptr<DataSource> vertexSource{};
		std::unique_ptr<DataSource> fragmentSource{};

		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		void use();

		Program(OpenglContext& openglContext);

		Program(Program const&) = delete;
		Program& operator=(Program const&) = delete;

		Program(Program&& other);
		Program& operator=(Program&& other);

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    std::unique_ptr<DataSource> vertexSource,
		    std::unique_ptr<DataSource> fragmentSource
		);

		std::optional<Program> reload(OpenglContext& openglContext);
	};
}