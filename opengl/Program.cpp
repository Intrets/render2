#include "render/opengl/Program.h"

#include "render/opengl/OpenglContext.h"

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

		this->openglContext.moveProgram(other, *this);
	}

	Program& Program::operator=(Program&& other) {
		assert(this->ID != other.ID);
		if (this->ID.data != 0) {
			this->openglContext.unRegisterProgram(*this);
			glDeleteProgram(this->ID.data);
		}

		this->ID = other.ID;
		other.ID = {};
		this->name = other.name;
		other.name = {};

		this->openglContext.moveProgram(other, *this);

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

		GLchar const* vertexPointer = &vertexDataSpan.front();

		if (vertexDataSpan.size() > std::numeric_limits<GLint>::max()) {
			return std::nullopt;
		}
		GLint vertexSize = static_cast<GLint>(vertexDataSpan.size());

		// Compile Vertex Shader
		glShaderSource(vertexShader.ID, 1, &vertexPointer, &vertexSize);
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

		GLchar const* fragmentPointer = &fragmentDataSpan.front();

		if (fragmentDataSpan.size() > std::numeric_limits<GLint>::max()) {
			return std::nullopt;
		}
		GLint fragmentSize = static_cast<GLint>(fragmentDataSpan.size());

		// Compile Fragment Shader
		glShaderSource(fragmentShader.ID, 1, &fragmentPointer, &fragmentSize);
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

		return Program(openglContext, ProgramID);
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