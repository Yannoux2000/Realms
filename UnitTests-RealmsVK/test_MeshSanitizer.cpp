#include "pch.h"

#include "Module/Graphics/MeshNameSanitizer.cpp"

TEST (testMeshSanitizer, SanitizeExemples) {
	ASSERT_EQ (rlms::MeshNameSanitizer::Sanitize ("Salut Tout Les Moins dE	18."), std::string ("salut_tout_les_moins_de_18"));
}

TEST (testMeshSanitizer, Sanitize_s41u_7) {
	ASSERT_EQ (rlms::MeshNameSanitizer::Sanitize ("		  S41u 7"), "_s41u_7");
}

TEST (testMeshSanitizer, SanitizeSpaces) {
	ASSERT_EQ (rlms::MeshNameSanitizer::Sanitize (" "), "_");
}

TEST (testMeshSanitizer, SanitizeTabs) {
	ASSERT_EQ (rlms::MeshNameSanitizer::Sanitize ("				"), "_");
}

TEST (testMeshSanitizer, SanitizeLong) {
	ASSERT_EQ (rlms::MeshNameSanitizer::Sanitize ("Sur cette base on peu donc ensuite élaborer"), "sur_cette_base_on_peu_donc_ensu");
}

TEST (testMeshSanitizer, GetAliasExemples) {
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("Models/Default/Air.vox"), std::string ("Air"));
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("Models / Default / Air.vox"), std::string (" Air"));
}

TEST (testMeshSanitizer, GetAliasExemples2) {
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("//Default / Air.vox"), std::string (" Air"));
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("//qsdqsd/dfgdfgdf/../ Air.vox"), std::string (" Air"));
}

TEST (testMeshSanitizer, GetAliasExemples3) {
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("Default / Air.vox"), std::string (" Air"));
	ASSERT_EQ (rlms::MeshNameSanitizer::GetAlias ("qsdqsd/dfgdfgdf/../ Air.vox"), std::string (" Air"));
}


TEST (testMeshSanitizer, IncrementAliasExemples) {
	ASSERT_EQ (rlms::MeshNameSanitizer::IncrementAlias ("sur_cette_base_on_peu_donc_ensu"), std::string ("sur_cette_base_on_peu_donc_en_0"));
}

TEST (testMeshSanitizer, IncrementAliasExemples3) {
	ASSERT_EQ (rlms::MeshNameSanitizer::IncrementAlias ("base"), std::string ("base_0"));
}

// WTF REGEX EST UN CONNARD
//TEST (testMeshSanitizer, IncrementAliasExemples2) {
//	ASSERT_EQ (rlms::MeshNameSanitizer::IncrementAlias ("sur_cette_base_on_peu_donc_en_0"), std::string ("sur_cette_base_on_peu_donc_en_1"));
//}
//
//TEST (testMeshSanitizer, IncrementAliasExemples4) {
//	ASSERT_EQ (rlms::MeshNameSanitizer::IncrementAlias ("base_9"), std::string ("base_10"));
//}
