#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <unordered_map>

#include <tepp/optional_ref.h>

namespace render::opengl
{
	struct Program;

	struct ProgramDescription
	{
		Program* program;
		bool autoReload = false;
	};

	struct ProgramRegistry
	{
		std::unordered_map<GLuint, ProgramDescription> programs{};

		void registerProgram(Program& program);
		void registerProgram(Program& program, ProgramDescription description);
		std::optional<ProgramDescription> unRegisterProgram(Program& program);

		te::optional_ref<Program> lookup(GLuint ID);
	};
}
