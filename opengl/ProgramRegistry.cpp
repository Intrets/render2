#include "render/opengl/ProgramRegistry.h"

#include "render/opengl/Program.h"

namespace render::opengl
{
	void ProgramRegistry::registerProgram(Program& program) {
		if (program.ID.data != 0) {
			assert(!this->programs.contains(program.ID.data));
			this->programs[program.ID.data].program = &program;
		}
	}

	void ProgramRegistry::unRegisterProgram(Program& program) {
		if (program.ID.data != 0) {
			assert(this->programs[program.ID.data].program == &program);
			this->programs.erase(program.ID.data);
		}
	}

	void ProgramRegistry::moveProgram(Program& from, Program& to) {
		this->unRegisterProgram(from);
		if (to.ID.data != 0) {
			assert(this->programs[to.ID.data].program == &from);
			this->programs[to.ID.data].program = &to;
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
