#pragma once

#include <exception>
#include <string>
#include "OpenGL.h"

class GLException : public std::exception {
private:
	std::string m_type;
public:
	GLException () noexcept : m_type ("Undefined error") {};
	GLException (std::string t_type) noexcept : m_type (t_type) {};
	virtual const char* what () const noexcept {
		return m_type.c_str ();
	};
};

struct GLInvalidEnum : public GLException {
	GLInvalidEnum () noexcept : GLException ("GL_INVALID_ENUM") {};
};

struct GLInvalidValue : public GLException {
	GLInvalidValue () noexcept : GLException ("GL_INVALID_VALUE") {};
};

struct GLInvalidOperation : public GLException {
	GLInvalidOperation () noexcept : GLException ("GL_INVALID_OPERATION") {};
};

struct GLInvalidFramebufferOperation : public GLException {
	GLInvalidFramebufferOperation () noexcept : GLException ("GL_INVALID_FRAMEBUFFER_OPERATION") {};
};

struct GLOutOfMemory : public GLException {
	GLOutOfMemory () noexcept : GLException ("GL_OUT_OF_MEMORY") {};
};

struct GLStackUnderflow : public GLException {
	GLStackUnderflow () noexcept : GLException ("GL_STACK_UNDERFLOW") {};
};

struct GLStackOverflow : public GLException {
	GLStackOverflow () noexcept : GLException ("GL_STACK_OVERFLOW") {};
};

void openGL_Error_Poll ();