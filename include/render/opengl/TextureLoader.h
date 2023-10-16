#pragma once

namespace resources
{
	struct Buffer;
}

namespace render::opengl
{
	struct Opengl2DTexture;
	struct OpenglContext;

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer);
}