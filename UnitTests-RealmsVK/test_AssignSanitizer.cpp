#include "pch.h"

#include "Module/Input/AssignSanitizer.cpp"

TEST (AssignSanitizer, IsGlobalNominal) {
	ASSERT_TRUE (rlms::AssignSanitizer::IsGlobal ("sample"));
	ASSERT_TRUE (rlms::AssignSanitizer::IsGlobal ("::"));
	ASSERT_FALSE (rlms::AssignSanitizer::IsGlobal ("sample::sample"));
	ASSERT_FALSE (rlms::AssignSanitizer::IsGlobal ("_::_"));
	ASSERT_TRUE (rlms::AssignSanitizer::IsGlobal (" sample :: sample "));
	ASSERT_FALSE (rlms::AssignSanitizer::IsGlobal ("sample::sample"));
}