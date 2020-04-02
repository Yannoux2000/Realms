#pragma once

#include <map>
#include <string>
#include "../../CoreTypes.h"
#include "AssignAddress.h"
#include "AssignSanitizer.h"

namespace rlms {
	class AssignDictionnary {
	private:
		std::map<std::string, INPUT_ADDRESS_SUB_TYPE> dict;
		INPUT_ADDRESS_SUB_TYPE _counter;
	public:
		static constexpr INPUT_ADDRESS_SUB_TYPE NULL_ADDR = 0;

		AssignDictionnary () : dict ({}), _counter (1) {}

		void reset () {
			while (!dict.empty ()) {
				auto it = dict.begin ();
				dict.erase (it);
			}
			_counter = 1;
		}

		void set (std::string const& name) {
			std::string map = AssignSanitizer::GetMapName (name);
			std::string input = AssignSanitizer::GetInputName (name);
			std::map<std::string, INPUT_ADDRESS_SUB_TYPE>::iterator it;

			it = dict.find (map);
			if (it == dict.end ()) {
				dict[map] = _counter;
			}

			it = dict.find (input);
			if (it == dict.end ()) {
				dict[input] = _counter;
			}
		}

		bool has (std::string name) {
			AssignAddress ret{ 0, 0 };

			std::map<std::string, INPUT_ADDRESS_SUB_TYPE>::iterator it;
			it = dict.find (AssignSanitizer::GetMapName (name));
			if (it != dict.end ()) {
				ret.map = it->second;
			}

			it = dict.find (AssignSanitizer::GetInputName (name));
			if (it != dict.end ()) {
				ret.input = it->second;
			}

			return (ret.map != 0 || ret.input != 0);
		}

		bool valid (AssignAddress const& ret) {
			return (ret.map != 0 || ret.input != 0);
		}

		AssignAddress get (std::string name) {
			AssignAddress ret{ 0, 0 };

			std::map<std::string, INPUT_ADDRESS_SUB_TYPE>::iterator it;
			it = dict.find (AssignSanitizer::GetMapName (name));
			if (it != dict.end ()) {
				ret.map = it->second;
			}

			it = dict.find (AssignSanitizer::GetInputName (name));
			if (it != dict.end ()) {
				ret.input = it->second;
			}
			return ret;
		}

		INPUT_ADDRESS_SUB_TYPE getWord (std::string name) {
			std::map<std::string, INPUT_ADDRESS_SUB_TYPE>::iterator it;
			it = dict.find(AssignSanitizer::NameSanitizer (name));
			if (it != dict.end ()) {
				return it->second;
			} else return NULL_ADDR;

		}

	};
}

