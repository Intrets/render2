#pragma once

#include <gl/glew.h>

namespace gli
{
	class texture;
}

namespace render::opengl
{
	struct Opengl2DTexture;
	struct Opengl2DArrayTexture;
	struct OpenglContext;

	//Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Buffer const& buffer, bool SRGB = true);
	//Opengl2DTexture load2DTexture(OpenglContext& openglContext, resources::Resource resource, bool SRGB = true);

	//Opengl2DArrayTexture load2DArrayTexture(OpenglContext& openglContext, resources::Buffer const& buffer, bool SRGB = true);
	//Opengl2DArrayTexture load2DArrayTexture(OpenglContext& openglContext, resources::Resource resource, bool SRGB = true);

	namespace impl
	{
		using LoadTextureResult = std::optional<std::variant<Opengl2DTexture, Opengl2DArrayTexture>>;

		LoadTextureResult loadTexture(
		    OpenglContext& openglContext,
		    gli::texture const& Texture,
		    bool SRGB = true
		);
	}

	//Opengl2DTexture load2DTextureMipmaps(OpenglContext& openglContext, std::vector<resources::Resource*> resources);
}