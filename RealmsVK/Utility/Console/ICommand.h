#pragma once

#include <vector>
#include <string>

class ICommand {
private:
	std::string _name;
	std::string _error;
	std::vector<std::string> arguments;
protected:
	void name (const std::string& name) {
		_name = name;
	}

	void error (const std::string& error) {
		_error = error;
	}

public:
	ICommand () : _name ("NoCommand"), _error ("") {}
	virtual ~ICommand () {};

	const std::string& name () {
		return _name;
	}

	const std::string& error () {
		return _error;
	}

	virtual bool prepare (std::string args ) {
		if (!args.empty ()) {
			error ("Invalid amount of arguments for this command!");
			return false;
		}
		return true;
	}

	/*
	returns a string describing the command, and it's usage
	*/
	virtual const std::string& help () {
		return "This command does nothing\n it takes no arguments";
	};

	/*
	Executes the command make sure you have inputed the arguments before you call it.
	*/
	virtual void exec () {};
};

