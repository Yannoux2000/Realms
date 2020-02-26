#pragma once
#include <string>

namespace rlms {

	class Exception : public std::exception {
	private:
		std::string m_type;
		std::string m_description;

	public:
		Exception () noexcept : m_type ("Undefined error"), m_description("this error has no description. guess that's a spot for an easter egg ?") {};

		Exception (std::string t_type) noexcept : m_type (t_type), m_description ("this error has no description. guess that's a spot for an easter egg ?") {};
		Exception (std::string t_type, std::string t_description) noexcept : m_type (t_type), m_description (t_description) {};

		virtual const char* what () const noexcept {
			return m_type.c_str ();
		};

		virtual const char* more () const noexcept {
			return m_description.c_str ();
		};
	};
}