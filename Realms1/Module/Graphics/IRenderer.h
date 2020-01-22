#pragma once
#include "../../Base/Logging/ILogged.h"
#include "IShaderProgram.h"
#include "IDrawable.h"

#include <memory>

namespace rlms {
	class IRenderer : public ILogged {
	protected:
		IShaderProgram* m_shader;
		size_t vertex_count;
	public:

		IRenderer ();
		~IRenderer ();

		IShaderProgram* getIShader ();

		void start (std::shared_ptr<Logger> funnel = nullptr);

		virtual bool load () { return false; };
		virtual void unload () {};

		void use ();
		void bind (IDrawable drawable);
		void bind (IDrawable* drawable);
		void draw (bool const single = false);
		void unbind ();

	};
}
