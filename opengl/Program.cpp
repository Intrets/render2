#include "render/opengl/Program.h"

#include "render/opengl/OpenglContext.h"
#include "render/opengl/Uniforms.h"

#include <fstream>

namespace render::opengl
{
	std::span<char const> DynamicData::get() const {
		return std::span(this->data.data(), this->data.size());
	}

	std::span<char const> ReferenceData::get() const {
		return this->data;
	}

	FileSource::FileSource(std::filesystem::path path_)
	    : path(path_) {
	}

	std::optional<DataSourceType> FileSource::data() const {
		std::ifstream file(this->path, std::ios::in);

		if (!file.good()) {
			return std::nullopt;
		}

		auto begin = file.tellg();
		file.seekg(0, std::ios::end);
		auto fileSize = file.tellg() - begin;

		file.seekg(0, std::ios::beg);

		auto dynamicData = std::make_unique<DynamicData>();

		dynamicData->data.resize(fileSize);
		file.read(dynamicData->data.data(), fileSize);

		return dynamicData;
	}

	std::unique_ptr<DataSource> FileSource::copy() const {
		auto result = std::make_unique<FileSource>();
		result->path = this->path;
		return result;
	}

	std::unique_ptr<DataSource> FileSource::make(std::filesystem::path const& path) {
		return std::make_unique<FileSource>(path);
	}

	std::optional<DataSourceType> StaticSource::data() const {
		auto referenceData = std::make_unique<ReferenceData>();
		referenceData->data = this->staticData.data;

		return referenceData;
	}

	std::unique_ptr<DataSource> StaticSource::copy() const {
		auto result = std::make_unique<StaticSource>();
		result->staticData = this->staticData;
		return result;
	}

	std::optional<DataSourceType> DynamicSource::data() const {
		auto referenceData = std::make_unique<ReferenceData>();
		referenceData->data = this->dynamicData.get();

		return referenceData;
	}

	std::unique_ptr<DataSource> DynamicSource::copy() const {
		auto result = std::make_unique<DynamicSource>();
		result->dynamicData = this->dynamicData;
		return result;
	}

	int32_t Program::getNextSampler() {
		return this->samplerCount++;
	}

	void Program::registerUniform(UniformBase& uniform) {
		auto refresh = this->uniformReferences.contains(uniform.getName());
		this->uniformReferences[std::string(uniform.getName())] = &uniform;

		if (!refresh) {
			this->sortedUniforms = false;
			this->uniformList.push_back(&uniform);
		}
	}

	std::span<UniformBase*> Program::getUniformsSorted() const {
		if (!this->sortedUniforms) {
			std::ranges::sort(this->uniformList, [](UniformBase* left, UniformBase* right) {
				return left->location < right->location;
			});
			this->sortedUniforms = true;
		}

		return this->uniformList;
	}

	void Program::use() {
		this->openglContext.use(*this);
	}

	Program::Program(OpenglContext& openglContext_)
	    : openglContext(openglContext_) {
	}

	Program::Program(OpenglContext& openglContext_, GLuint ID_)
	    : openglContext(openglContext_) {
		this->ID.data = ID_;
		this->ID.qualifier = this->openglContext.getQualifier();
		this->openglContext.registerProgram(*this);
	}

	Program::Program(Program&& other)
	    : openglContext(other.openglContext) {
		this->ID = other.ID;
		other.ID = {};
		this->name = other.name;
		other.name = {};

		this->vertexInfos = std::move(other.vertexInfos);

		this->shaderSourceGenerators = std::move(other.shaderSourceGenerators);

		this->openglContext.registerProgram(*this);
	}

	Program& Program::operator=(Program&& other) {
		assert(this->ID != other.ID);

		auto description = std::invoke([&]() -> std::optional<ProgramDescription> {
			if (this->ID.data != 0) {
				auto result = this->openglContext.unRegisterProgram(*this);
				glDeleteProgram(this->ID.data);
				return result;
			}
			else {
				return std::nullopt;
			}
		});

		this->ID = other.ID;
		other.ID = {};
		this->name = other.name;
		other.name = {};

		this->vertexInfos = std::move(other.vertexInfos);

		this->shaderSourceGenerators = std::move(other.shaderSourceGenerators);

		if (description.has_value()) {
			this->openglContext.registerProgram(*this, description.value());
		}
		else {
			this->openglContext.registerProgram(*this);
		}

		return *this;
	}

	Program::~Program() {
		this->openglContext.unRegisterProgram(*this);
		glDeleteProgram(this->ID.data);
		this->ID = {};
	}

	std::optional<Program> Program::load(OpenglContext& openglContext, std::span<char const> vertexDataSpan, std::span<char const> fragmentDataSpan) {
		auto vertexShader = Shader::makeVertexShader();
		auto fragmentShader = Shader::makeFragmentShader();

		GLint Result = GL_FALSE;
		int32_t InfoLogLength = 0;

		if (vertexDataSpan.size() > std::numeric_limits<GLint>::max()) {
			return std::nullopt;
		}

		// Compile Vertex Shader
		openglContext.setShaderSource(vertexShader.ID, std::string_view(vertexDataSpan.data(), vertexDataSpan.size()));
		glCompileShader(vertexShader.ID);

		// Check Vertex Shader
		glGetShaderiv(vertexShader.ID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(vertexShader.ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(vertexShader.ID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			openglContext.logError("Vertex shader error:\n{}\n", &VertexShaderErrorMessage[0]);
			return std::nullopt;
		}

		if (fragmentDataSpan.size() > std::numeric_limits<GLint>::max()) {
			return std::nullopt;
		}

		// Compile Fragment Shader
		openglContext.setShaderSource(fragmentShader.ID, std::string_view(fragmentDataSpan.data(), fragmentDataSpan.size()));
		glCompileShader(fragmentShader.ID);

		// Check Fragment Shader
		glGetShaderiv(fragmentShader.ID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(fragmentShader.ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(fragmentShader.ID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			openglContext.logError("Fragment shader error:\n{}\n", &FragmentShaderErrorMessage[0]);
			return std::nullopt;
		}

		// Link the program
		openglContext.logInfo("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, vertexShader.ID);
		glAttachShader(ProgramID, fragmentShader.ID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			openglContext.logError("Program error:\n{}\n", &ProgramErrorMessage[0]);
			return std::nullopt;
		}

		glDetachShader(ProgramID, vertexShader.ID);
		glDetachShader(ProgramID, fragmentShader.ID);

		openglContext.logInfo("Program ID: {}\n", ProgramID);

		auto result = Program(openglContext, ProgramID);

		GLint vertexCount = 0;
		glGetProgramiv(result.ID.data, GL_ACTIVE_ATTRIBUTES, &vertexCount);

		GLint nameLength = 0;
		glGetProgramiv(result.ID.data, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLength);

		std::string vertexNameBuffer{};
		vertexNameBuffer.resize(nameLength);

		for (integer_t i = 0; i < static_cast<integer_t>(vertexCount); i++) {
			GLsizei actualNameLength = 0;
			GLint size = 0;
			GLenum type = 0;
			glGetActiveAttrib(result.ID.data, static_cast<GLuint>(i), nameLength, &actualNameLength, &size, &type, vertexNameBuffer.data());

			std::string_view vertexName = std::string_view(vertexNameBuffer.data(), actualNameLength);

			auto& info = result.vertexInfos.emplace_back();
			info.index = i;
			info.name = vertexName;
#ifdef WRANGLE_GLESv3
#define TYPE_LIST(X) X(GL_FLOAT_MAT2x3) X(GL_FLOAT_MAT2x4) X(GL_FLOAT_MAT3x2) X(GL_FLOAT_MAT3x4) X(GL_FLOAT_MAT4x2) X(GL_FLOAT_MAT4x3) X(GL_UNSIGNED_INT_VEC2) X(GL_UNSIGNED_INT_VEC3) X(GL_UNSIGNED_INT_VEC4)
#else
#define TYPE_LIST(X) X(GL_FLOAT_MAT2x3) X(GL_FLOAT_MAT2x4) X(GL_FLOAT_MAT3x2) X(GL_FLOAT_MAT3x4) X(GL_FLOAT_MAT4x2) X(GL_FLOAT_MAT4x3) X(GL_UNSIGNED_INT_VEC2) X(GL_UNSIGNED_INT_VEC3) X(GL_UNSIGNED_INT_VEC4) X(GL_DOUBLE) X(GL_DOUBLE_VEC2) X(GL_DOUBLE_VEC3) X(GL_DOUBLE_VEC4) X(GL_DOUBLE_MAT2) X(GL_DOUBLE_MAT3) X(GL_DOUBLE_MAT4) X(GL_DOUBLE_MAT2x3) X(GL_DOUBLE_MAT2x4) X(GL_DOUBLE_MAT3x2) X(GL_DOUBLE_MAT3x4) X(GL_DOUBLE_MAT4x2) X(GL_DOUBLE_MAT4x3)
#endif

#define DO_CASE(NAME) \
	case NAME: \
		return #NAME;

			info.type = std::invoke([&] {
				switch (type) {
					TYPE_LIST(DO_CASE)
					case GL_FLOAT:
						return "float";
					case GL_FLOAT_VEC2:
						return "vec2";
					case GL_FLOAT_VEC3:
						return "vec3";
					case GL_FLOAT_VEC4:
						return "vec4";
					case GL_FLOAT_MAT2:
						return "mat2";
					case GL_FLOAT_MAT3:
						return "mat3";
					case GL_FLOAT_MAT4:
						return "mat4";
					case GL_INT:
						return "int";
					case GL_INT_VEC2:
						return "ivec2";
					case GL_INT_VEC3:
						return "ivec3";
					case GL_INT_VEC4:
						return "ivec4";
					case GL_UNSIGNED_INT:
						return "uint";
					default:
						tassert(0);
						return "";
				}
			});
		}

		return result;
	}

	std::optional<Program> Program::load(
	    OpenglContext& openglContext,
	    std::unique_ptr<DataSource> vertexSource,
	    std::unique_ptr<DataSource> fragmentSource
	) {
		openglContext.logInfo("Compiling Shaders\n");

		auto vertexData = vertexSource->data();
		auto fragmentData = fragmentSource->data();

		if (!vertexData.has_value()) {
			openglContext.logError("Failed to get vertex data source.\n");
			return std::nullopt;
		}

		if (!fragmentData.has_value()) {
			openglContext.logError("Failed to get fragment data source.\n");
			return std::nullopt;
		}

		auto vertexDataSpan = vertexData.value()->get();
		auto fragmentDataSpan = fragmentData.value()->get();

		return Program::load(openglContext, vertexDataSpan, fragmentDataSpan);
	}

	std::optional<Program> Program::load(OpenglContext& openglContext, BufferGenerator vertexSourceGenerator, BufferGenerator fragmentSourceGenerator) {
		auto vertexSource = vertexSourceGenerator();
		auto fragmentSource = fragmentSourceGenerator();

		if (!vertexSource.has_value() || !fragmentSource.has_value()) {
			return std::nullopt;
		}
		else {
			if (auto program = Program::load(openglContext, vertexSource->get()->getSpan<char>(), fragmentSource->get()->getSpan<char>())) {
				program->shaderSourceGenerators = BufferGenerators{
					.vertexGenerator = std::move(vertexSourceGenerator),
					.fragmentGenerator = std::move(fragmentSourceGenerator),
				};

				return std::move(program.value());
			}
		}

		return std::nullopt;
	}

	void Program::reload(Program& program) {
		if (program.shaderSourceGenerators.has_value()) {
			auto newProgram = Program::load(program.openglContext, program.shaderSourceGenerators->vertexGenerator, program.shaderSourceGenerators->fragmentGenerator);
			if (newProgram.has_value()) {
				newProgram->name = program.name;
				program = std::move(newProgram.value());
			}
		}
	}

	Shader::Shader(Shader&& other) {
		this->ID = other.ID;
		other.ID = 0;
	}

	Shader& Shader::operator=(Shader&& other) {
		if (this->ID == 0) {
			glDeleteShader(this->ID);
		}

		this->ID = other.ID;
		other.ID = 0;

		return *this;
	}

	Shader::~Shader() {
		glDeleteShader(this->ID);
	}

	Shader Shader::makeVertexShader() {
		Shader result{};

		result.ID = glCreateShader(GL_VERTEX_SHADER);

		return result;
	}

	Shader Shader::makeFragmentShader() {
		Shader result{};

		result.ID = glCreateShader(GL_FRAGMENT_SHADER);

		return result;
	}
}