#pragma once

#include <ctime>
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include <memory>

namespace rlms {
#define CSETTING_MAX_DATE_LEN 100

	enum class SettingType {
		String, Int, Float, Boolean, MemorySize, DateTime, Array
	};

	class SettingArray; // forward declaration needed by SettingValue::create_array

	// Defines the abstract class for a Setting primitive.
	class SettingValue {
	private:
		SettingType _type;
	public:
		SettingValue (SettingType type);

		// Returns the type of this Setting value
		SettingType type () const;

		// Factory methods
		static std::unique_ptr<SettingValue> create_string (std::string str);
		static std::unique_ptr<SettingValue> create_int (std::int64_t val);
		static std::unique_ptr<SettingValue> create_float (double val);
		static std::unique_ptr<SettingValue> create_boolean (bool val);
		static std::unique_ptr<SettingValue> create_datetime (tm val);
		static std::unique_ptr<SettingValue> create_array ();

		template<typename InputIterator>
		static std::unique_ptr<SettingValue> create_array (InputIterator begin, InputIterator end) {
			return std::unique_ptr<SettingValue> (new SettingArray (begin, end));
		}

		// Equality tests
		virtual bool equals (std::string) const;
		virtual bool equals (const char*) const;
		virtual bool equals (std::int64_t) const;
		virtual bool equals (int) const;
		virtual bool equals (float) const;
		virtual bool equals (bool) const;

		// To string method
		virtual std::string to_string () const = 0;
	};

	class SettingString : public SettingValue {
	private:
		std::string _val;
	public:
		explicit SettingString (std::string val);

		// Returns the string value
		std::string value () const;

		bool equals (std::string val) const;
		bool equals (const char* val) const;

		std::string to_string () const;
	};

	class SettingInt : public SettingValue {
	private:
		std::int64_t _val; // 64 bit integer
	public:
		explicit SettingInt (std::int64_t val);

		// Returns the integer value
		std::int64_t value () const;

		bool equals (std::int64_t val) const;
		bool equals (int val) const;

		std::string to_string () const;
	};

	class SettingFloat : public SettingValue {
	private:
		double _val;
	public:
		explicit SettingFloat (double val);

		// Returns the floating point value
		double value () const;

		bool equals (float val) const;
		std::string to_string () const;
	};

	class SettingBoolean : public SettingValue {
	private:
		bool _val;
	public:
		explicit SettingBoolean (bool val);

		// Returns the boolean value
		bool value () const;

		bool equals (bool val) const;
		std::string to_string () const;
	};

	class SettingDateTime : public SettingValue {
	private:
		time_t _val;
	public:
		explicit SettingDateTime (tm val);

		// Returns the time value
		time_t value () const;

		std::string to_string () const;
	};

	// Cast a shared_ptr of a SettingValue into its primitive type
	template <class T>
	T setting_value_cast (std::shared_ptr<SettingValue> value) {
		if (value->type () == SettingType::Boolean)
			return static_cast<T>(std::static_pointer_cast<SettingBoolean>(value)->value ());
		else if (value->type () == SettingType::Int)
			return static_cast<T>(std::static_pointer_cast<SettingInt>(value)->value ());
		else if (value->type () == SettingType::Float)
			return static_cast<T>(std::static_pointer_cast<SettingFloat>(value)->value ());
		else if (value->type () == SettingType::DateTime)
			return static_cast<T>(std::static_pointer_cast<SettingDateTime>(value)->value ());
		return T ();
	}

	// Template specialization for std::string
	template <>
	inline std::string setting_value_cast<std::string>(std::shared_ptr<SettingValue> value) {
		return std::static_pointer_cast<SettingString>(value)->value ();
	}

	class SettingArray : public SettingValue {
	private:
		std::vector<std::shared_ptr<SettingValue>> _array;
	public:
		typedef std::vector<std::shared_ptr<SettingValue>>::const_iterator const_iterator;

		// Create empty Setting array
		SettingArray ();

		// Create a Setting array from iterators
		template <typename InputIterator>
		SettingArray (InputIterator begin, InputIterator end) : _array (begin, end) {}

		// Add values to the Setting array
		void add (std::unique_ptr<SettingValue> v);
		void add (std::shared_ptr<SettingValue> v);

		// Iterate through the Setting array
		const_iterator cbegin () const;
		const_iterator cend () const;

		// Return all items in the array as values in a vector
		template <class T>
		std::vector<T> as_vector () const {
			std::vector<T> list;

			for (auto it = cbegin (); it != cend (); ++it) {
				list.push_back (setting_value_cast<T>(it));
			}

			return list;
		}

		// Get the size of the array
		size_t size () const {
			return _array.size ();
		}

	// Random access
		std::shared_ptr<SettingValue> at (const int index = 0) const;
		std::shared_ptr<SettingValue> operator[] (const int index) const;

		std::string to_string () const;
	};
}