cmake_minimum_required (VERSION 3.16)

set(MODULE_NAME render)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		opengl/ManagedTexture
		opengl/ProgramRegistry
		RenderInfoBase
		Color
		DataType
		Renderer
		opengl/Wrappers
		opengl/Program
		opengl/OpenglContext
		opengl/OpenglFramebuffer
		opengl/OpenglVAO
		opengl/OpenglVBO
		opengl/OpenglPBO
		opengl/OpenglTexture
		opengl/OpenglBufferTexture
		opengl/BufferUsageHint
		opengl/BufferTarget
		opengl/TextureTarget
		opengl/TextureLoader
		opengl/Qualifier
		opengl/Uniforms
		Convert
		SRGBConversion
		CachedValue
	CXX_STANDARD 23
	REQUIRED_LIBS
		tepp
		wrangled_gl
		misc
		WGLM
		gli
		resource_data
)
