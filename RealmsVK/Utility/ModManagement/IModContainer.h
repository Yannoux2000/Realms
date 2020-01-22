#pragma once

namespace rlms {

	#define DLLEXP __declspec(dllexport) 
	#define ModContainer(name) DLLEXP name : public IModContainer

	class DLLEXP IModContainer {
	public:
		virtual bool load () = 0;
		virtual bool settings () = 0;
		virtual bool start () = 0;
		virtual bool stop () = 0;
	};
}