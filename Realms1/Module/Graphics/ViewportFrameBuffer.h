#pragma once
#include "OpenGL.h"
#include "AspectRatio.h"

namespace rlms{
	class ViewportFrameBuffer {
	private:
		GLuint m_fbo;
		GLuint m_rbo;
		GLuint m_texture;

	public:
		ViewportFrameBuffer () : m_fbo (), m_rbo (), m_texture () { }

		~ViewportFrameBuffer () { }

		void load () {
			glGenFramebuffers (1, &m_fbo);
			glBindFramebuffer (GL_FRAMEBUFFER, m_fbo);

			// create a color attachment texture
			glGenTextures (1, &m_texture);
			glBindTexture (GL_TEXTURE_2D, m_texture);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, AspectRatio::Width (), AspectRatio::Height (), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
			glGenRenderbuffers (1, &m_rbo);
			glBindRenderbuffer (GL_RENDERBUFFER, m_rbo);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, AspectRatio::Width (), AspectRatio::Height ()); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo); // now actually attach it
			// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
			// if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)

			glBindFramebuffer (GL_FRAMEBUFFER, 0);
		}

		void unload () {
			glDeleteFramebuffers (1, &m_fbo);
		}

		void enable () {
			// bind to framebuffer and draw scene as we normally would to color m_texture 
			glBindFramebuffer (GL_FRAMEBUFFER, m_fbo);
			glEnable (GL_DEPTH_TEST);
		}

		GLuint getTexture () {
			return m_texture;
		}

		void disable () {
			Disable ();
		}

		static inline void Disable () {
			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color m_texture
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glDisable (GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		}
	};
}

