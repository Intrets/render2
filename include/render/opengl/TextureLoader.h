#pragma once

namespace resources
{
	struct Buffer;
	struct Resource;
}

namespace render::opengl
{
	struct Opengl2DTexture;
	struct OpenglContext;

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer);
	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Resource resource);
}