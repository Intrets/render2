#pragma once

#include "render/Renderer.h"
#include "render/opengl/Program.h"

namespace render::opengl
{
	struct OpenglRenderer : Renderer
	{
		OpenglContext& openglContext;
		Program program;

		OpenglRenderer() = delete;
		OpenglRenderer(OpenglContext& openglContext);
		~OpenglRenderer() = default;

		bool reloadShaders(OpenglContext& openglContext);

		void addUniform(std::string_view name, render::DataType uniformDataType, uint32_t count) override;
	};
}