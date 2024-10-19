#include "render/opengl/ProgramRegistry.h"

#include "render/opengl/Program.h"

namespace render::opengl
{
	void ProgramRegistry::registerProgram(Program& program) {
		if (program.ID.data != 0) {
			this->programs[program.ID.data].program = &program;
		}
	}

	void ProgramRegistry::registerProgram(Program& program, ProgramDescription description_) {
		if (program.ID.data != 0) {
			auto& description = this->programs[program.ID.data];

			description = description_;
			description.program = &program;
		}
	}

	std::optional<ProgramDescription> ProgramRegistry::unRegisterProgram(Program& program) {
		auto it = this->programs.find(program.ID.data);
		if (it != this->programs.end()) {
			auto description = it->second;
			description.program = nullptr;
			this->programs.erase(it);
			return description;
		}
		else {
			return std::nullopt;
		}
	}

	te::optional_ref<Program> ProgramRegistry::lookup(GLuint ID) {
		auto it = this->programs.find(ID);
		if (it == this->programs.end()) {
			return te::nullopt;
		}
		else {
			return *it->second.program;
		}
	}
}
