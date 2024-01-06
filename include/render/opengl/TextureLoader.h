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

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer, bool SRGB = true);
	Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Resource resource, bool SRGB = true);

	Opengl2DTexture load2DTextureMipmaps(OpenglContext& openglContext, std::vector<resources::Resource*> resources);
}