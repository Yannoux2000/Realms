#pragma once
#include "ISystem.h"
namespace rlms{
	class IGameLoaderSystem : public ISystem {
	private:
		bool _done_loading;
		bool _done_saving;

	protected:
		void done_loading (bool const& done_loading) {
			_done_loading = done_loading;
		}

		void done_saving (bool const& done_saving) {
			_done_saving = done_saving;
		}

	public:
		IGameLoaderSystem () : _done_loading (false), _done_saving(false) {};
		~IGameLoaderSystem () {};
	
		bool done_loading () {
			return _done_loading; 
		};

		bool done_saving () {
			return _done_loading;
		};

		virtual void load () {};
		virtual void save () {};
	};
}