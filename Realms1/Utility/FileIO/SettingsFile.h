#pragma once

#include "SettingValue.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>


namespace rlms {
	struct SettingError {
		std::string message;
		int line_no;

		SettingError (const char* msg, int line) : message (msg), line_no (line) {}
	};

	class SettingDocument {
	private:
		// Stores the key-value pairs. The keys are the full key name (with period notation)
		std::unordered_map<std::string, std::shared_ptr<SettingValue>> values_;
	public:
		typedef std::unordered_map<std::string, std::shared_ptr<SettingValue>>::const_iterator const_iterator;

		void Print () const;

		// Iterate through each key
		const_iterator cbegin () const;
		const_iterator cend () const;

		// Inserts a new Setting value with a key
		void insert (std::string key, std::shared_ptr<SettingValue> value);
		void insert (std::string key, std::unique_ptr<SettingValue> value);

		// BitSet a Setting key to a value
		void set (std::string key, std::shared_ptr<SettingValue> value);
		void set (std::string key, std::unique_ptr<SettingValue> value);

		// Returns true if the key already exists
		bool is_key (std::string key) const;

		// Returns the Setting value for a particular key
		std::shared_ptr<SettingValue> get (std::string key) const;

		template <class T>
		std::shared_ptr<T> get (std::string key) const {
			if (!is_key (key)) return nullptr;
			return std::static_pointer_cast<T>(get (key));
		}

		template <class T>
		T get_as (std::string key) const {
			return setting_value_cast<T>(get (key));
		}

		template <class T>
		std::vector<T> get_array_as (std::string key) const {
			std::vector<T> array;

			std::shared_ptr<SettingValue> sec = get (key);
			auto t = std::static_pointer_cast<cSetting::SettingArray>(sec);
			for (size_t i = 0; i < t->size (); i++) {
				array.push_back (setting_value_cast<T>(t->at (i)));
			}

			return array;
		}

		// Writes Setting document to stream
		std::ostream& write (std::ostream& out);
	};

	class SettingParser {
	private:
		std::ifstream source_file_;
		char* cur_;
		int cur_line_;

		char cur () const {
			return *cur_;
		}

	// List of parse errors
		std::vector<SettingError> errors_;
		void error (const char* format, ...);

		bool is_whitespace (char c, bool new_line = false);
		bool is_numeric (char c);

		bool is_integer (std::string str);
		bool is_float (std::string str);
		bool is_datetime (std::string str);

		tm to_time (std::string str);

		void expect (char c);
		void advance (char c, bool new_line = false);
		void skip_whitespace (bool new_line = false);
		void skip_whitespace_and_comments ();

		char next_char ();
		char next_skip_whitespace (bool new_line = false);

		std::shared_ptr<SettingValue> parse_string ();
		std::shared_ptr<SettingValue> parse_number ();
		std::shared_ptr<SettingValue> parse_boolean ();
		std::shared_ptr<SettingValue> parse_array ();
		std::shared_ptr<SettingValue> parse_value ();

		std::string parse_key_group ();
		std::string parse_key ();
	public:
		SettingParser ();
		explicit SettingParser (const std::string filename);

		// Parse the document.
		SettingDocument parse ();

		// Returns true if the input file is valid
		bool good () const;

		// Returns true if no were no errors after parsing
		bool success () const;

		// Open a file. Returns true if good()
		bool open (const std::string filename);

		// Close file
		void close ();

		// Returns the number of errors
		size_t num_errors () const {
			return errors_.size ();
		}

	// Returns the ith error
		SettingError get_error (int i) const {
			return errors_[i];
		}
	};
}

