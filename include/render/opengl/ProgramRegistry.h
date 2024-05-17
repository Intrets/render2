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
	};

	struct ProgramRegistry
	{
		std::unordered_map<GLuint, ProgramDescription> programs{};

		void registerProgram(Program& program);
		void unRegisterProgram(Program& program);
		void moveProgram(Program& from, Program& to);

		te::optional_ref<Program> lookup(GLuint ID);
	};
}
