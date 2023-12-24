#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <vector>

#include <wrangled_gl/wrangled_gl.h>

#include "render/opengl/Qualifier.h"

#include <resources/Resources.h>

namespace resources
{
	struct Resource;
}

namespace render::opengl
{
	struct OpenglContext;

	struct Data
	{
		virtual std::span<char const> get() const = 0;

		Data() = default;
		virtual ~Data() = default;
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

		DataSource() = default;
		virtual ~DataSource() = default;
	};

	struct FileSource : DataSource
	{
		std::filesystem::path path{};

		FileSource() = default;
		FileSource(std::filesystem::path path_);

		std::optional<DataSourceType> data() const override;
		std::unique_ptr<DataSource> copy() const override;
		static std::unique_ptr<DataSource> make(std::filesystem::path path);
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
		std::optional<resources::Resource> vertexSource{};
		std::optional<resources::Resource> fragmentSource{};

		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		int32_t samplerCount = 0;

		int32_t getNextSampler();

		void use();

		Program(OpenglContext& openglContext);

		Program(Program const&) = delete;
		Program& operator=(Program const&) = delete;

		Program(Program&& other);
		Program& operator=(Program&& other);

		~Program();

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    std::span<char const> vertexSource,
		    std::span<char const> fragmentSource
		);

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    std::unique_ptr<DataSource> vertexSource,
		    std::unique_ptr<DataSource> fragmentSource
		);

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    resources::Resource vertexSource,
		    resources::Resource fragmentSource
		);
	};
}