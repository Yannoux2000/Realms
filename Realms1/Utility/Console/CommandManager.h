#pragma once

#include <array>
#include <map>
#include <string>

#include "IO/ILogged.h"
#include "ICommand.h"

class CommandManager {
private:
	static std::array <std::string, 64> _history;
	static std::map<std::string, ICommand*> _commandList;

	static std::string format_cmd_name (std::string cmd_name) {
		std::string formated_name = "";

		size_t prev = 0, cur = 0;

		while (formated_name.empty ()) {
			prev = cur;
			cur = cmd_name.find (' ', cur);
			formated_name = cmd_name.substr (prev, cur);
		};

		return formated_name;
	}

public:
	static std::string Parse (std::string& command) {

		std::string fmt_name = command;

		if (fmt_name.empty ()) {
			return "Error: command was not recognisable.";
		}

		auto it = _commandList.find (fmt_name);

		if (it == _commandList.end ()) {
			return "Error: command not found.";
		}

		if (!it->second->prepare (command)) {
			return "Error: " + it->second->error ();
		}

		it->second->exec ();
	}

	static std::string ListCmds () {
		std::ostringstream oss;
		oss << "Full command list: \n";
		for (auto it = _commandList.begin (); it != _commandList.end (); it++) {
			oss << '\t' << it->first << "\n";
		}
		return oss.str ();
	}

	template<class C> static bool Register ();
};

template<class C> inline bool CommandManager::Register () {
	logger->tag (LogTags::Debug) << "Registering " << typeid(C).name () << "." << '\n';

	//template is a command
	if (!std::is_base_of<ICommand, C>::value && typeid(C) != typeid(ICommand)) {
		logger->tag (LogTags::Error) << "Command must inherit from ICommand !" << '\n';
		return false;
	}

	C* cmd = new C ();

}
