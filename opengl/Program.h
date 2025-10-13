#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include <wrangled_gl/wrangled_gl.h>

#include "render/opengl/Qualifier.h"

#include <tepp/cstring_view.h>
#include <tepp/span.h>
#include <tepp/string_key_unordered_map.h>

#include <resource_data/ResourceData.h>

namespace render::opengl
{
	struct OpenglContext;

	struct Data
	{
		virtual te::span<char const> get() const = 0;

		Data() = default;
		virtual ~Data() = default;
	};

	struct ReferenceData : Data
	{
		te::span<char const> data{};

		virtual te::span<char const> get() const override;
	};

	struct DynamicData : Data
	{
		std::vector<char> data{};

		virtual te::span<char const> get() const override;
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
		static std::unique_ptr<DataSource> make(std::filesystem::path const& path);
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

	struct BufferGenerators
	{
		BufferGenerator vertexGenerator{};
		BufferGenerator fragmentGenerator{};
	};

	struct UniformBase;

	struct VertexInfo
	{
		integer_t index{};
		std::string name{};
		std::string type{};
	};

	struct Program
	{
		te::cstring_view name{};
		OpenglContext& openglContext;
		Qualified<GLuint> ID{};

		std::optional<BufferGenerators> shaderSourceGenerators{};

		te::string_key_unordered_map<UniformBase*> uniformReferences{};
		mutable bool sortedUniforms = true;
		mutable std::vector<UniformBase*> uniformList{};

		std::vector<VertexInfo> vertexInfos{};

		int32_t samplerCount = 0;

		int32_t getNextSampler();

		void registerUniform(UniformBase& uniform);
		te::span<UniformBase*> getUniformsSorted() const;

		void use();

		Program(OpenglContext& openglContext);
		Program(OpenglContext& openglContext, GLuint ID_);

		Program(Program const&) = delete;
		Program& operator=(Program const&) = delete;

		Program(Program&& other);
		Program& operator=(Program&& other);

		~Program();

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    te::span<char const> vertexSource,
		    te::span<char const> fragmentSource
		);

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    std::unique_ptr<DataSource> vertexSource,
		    std::unique_ptr<DataSource> fragmentSource
		);

		static std::optional<Program> load(
		    OpenglContext& openglContext,
		    BufferGenerator vertexSourceGenerator,
		    BufferGenerator fragmentSourceGenerator
		);

		static void reload(Program& program);
	};
}
