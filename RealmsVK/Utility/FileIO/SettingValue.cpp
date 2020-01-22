#include "Settingvalue.h"

using namespace rlms;

SettingValue::SettingValue (SettingType type) : _type (type) {}

SettingType SettingValue::type () const {
	return _type;
}

std::unique_ptr<SettingValue> SettingValue::create_string (std::string val) {
	return std::unique_ptr<SettingValue> (new SettingString (val));
}

std::unique_ptr<SettingValue> SettingValue::create_int (std::int64_t val) {
	return std::unique_ptr<SettingValue> (new SettingInt (val));
}

std::unique_ptr<SettingValue> SettingValue::create_float (double val) {
	return std::unique_ptr<SettingValue> (new SettingFloat (val));
}

std::unique_ptr<SettingValue> SettingValue::create_boolean (bool val) {
	return std::unique_ptr<SettingValue> (new SettingBoolean (val));
}

std::unique_ptr<SettingValue> SettingValue::create_datetime (tm val) {
	return std::unique_ptr<SettingValue> (new SettingDateTime (val));
}

std::unique_ptr<SettingValue> SettingValue::create_array () {
	return std::unique_ptr<SettingValue> (new SettingArray ());
}

bool SettingValue::equals (std::string) const {
	return type () == SettingType::String;
}

bool SettingValue::equals (const char*) const {
	return type () == SettingType::String;
}

bool SettingValue::equals (std::int64_t) const {
	return type () == SettingType::Int;
}

bool SettingValue::equals (int) const {
	return type () == SettingType::Int;
}

bool SettingValue::equals (float) const {
	return type () == SettingType::Float;
}

bool SettingValue::equals (bool) const {
	return type () == SettingType::Boolean;
}

SettingString::SettingString (std::string val) : SettingValue (SettingType::String), _val (val) {}
SettingInt::SettingInt (std::int64_t val) : SettingValue (SettingType::Int), _val (val) {}
SettingFloat::SettingFloat (double val) : SettingValue (SettingType::Float), _val (val) {}
SettingBoolean::SettingBoolean (bool val) : SettingValue (SettingType::Boolean), _val (val) {}
SettingDateTime::SettingDateTime (tm val) : SettingValue (SettingType::DateTime) {
	_val = mktime (&val);
}
SettingArray::SettingArray () : SettingValue (SettingType::Array) {}

std::string SettingString::value () const {
	return _val;
}
std::int64_t SettingInt::value () const {
	return _val;
}
double SettingFloat::value () const {
	return _val;
}
bool SettingBoolean::value () const {
	return _val;
}
time_t SettingDateTime::value () const {
	return _val;
}

void SettingArray::add (std::unique_ptr<SettingValue> v) {
	_array.push_back (move (v));
}

void SettingArray::add (std::shared_ptr<SettingValue> v) {
	_array.push_back (v);
}

SettingArray::const_iterator SettingArray::cbegin () const {
	return _array.cbegin ();
}

SettingArray::const_iterator SettingArray::cend () const {
	return _array.cend ();
}

std::shared_ptr<SettingValue> SettingArray::at (const int index) const {
	return _array[index];
}

std::shared_ptr<SettingValue> SettingArray::operator[] (const int index) const {
	return at (index);
}

bool SettingString::equals (std::string val) const {
	return _val == val;
}

bool SettingString::equals (const char* val) const {
	return _val == std::string (val);
}

bool SettingInt::equals (std::int64_t val) const {
	return _val == val;
}

bool SettingInt::equals (int val) const {
	return _val == val;
}

bool SettingFloat::equals (float val) const {
	return _val == val;
}

bool SettingBoolean::equals (bool val) const {
	return _val == val;
}

std::string SettingString::to_string () const {
	return _val;
}

std::string SettingInt::to_string () const {
	return std::to_string (_val);
}

std::string SettingFloat::to_string () const {
	return std::to_string (_val);
}

std::string SettingBoolean::to_string () const {
	return _val ? "true" : "false";
}

std::string SettingDateTime::to_string () const {
	tm t;
	char buf[CSETTING_MAX_DATE_LEN];
	gmtime_s (&t, &_val);
	strftime (buf, CSETTING_MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", &t);
	return std::string (buf);
}

std::string SettingArray::to_string () const {
	std::string str = "[";
	for (int i = 0; i < (int)_array.size (); i++) {
		if (i) str += ", ";
		str += _array[i]->to_string ();
	}
	str += "]";

	return str;
}