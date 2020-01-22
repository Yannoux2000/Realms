#include "GLError.h"

void openGL_Error_Poll () {
	GLenum status = glGetError ();
	if (status != GL_NO_ERROR) {
		switch (status) {
			case GL_INVALID_ENUM:
				throw GLInvalidEnum ();
				break;
			case GL_INVALID_VALUE:
				throw GLInvalidValue ();
				break;
			case GL_INVALID_OPERATION:
				throw GLInvalidOperation ();
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				throw GLInvalidFramebufferOperation ();
				break;
			case GL_OUT_OF_MEMORY:
				throw GLOutOfMemory ();
				break;
			case GL_STACK_UNDERFLOW:
				throw GLStackUnderflow ();
				break;
			case GL_STACK_OVERFLOW:
				throw GLStackOverflow ();
				break;
		}
	}
}
