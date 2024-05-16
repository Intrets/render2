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

	Opengl2DTexture load2DTexture(OpenglContext& openglContext, std::span<char const> buffer, bool SRGB = true);

	Opengl2DArrayTexture load2DArrayTexture(OpenglContext& openglContext, std::span<char const> buffer, bool SRGB = true);

	namespace impl
	{
		using LoadTextureResult = std::optional<std::variant<Opengl2DTexture, Opengl2DArrayTexture>>;

		LoadTextureResult loadTexture(
		    OpenglContext& openglContext,
		    gli::texture const& Texture,
		    bool SRGB = true
		);
	}
}