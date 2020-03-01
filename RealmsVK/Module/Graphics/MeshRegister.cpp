#include "MeshRegister.h"

#include "../../Base/Exception.h"

#include "MeshNameSanitizer.h"

void rlms::MeshRegister::start (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_model_Allocator = std::unique_ptr<alloc_type> (new alloc_type (mesh_pool_size, alloc->allocate (mesh_pool_size)));

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void rlms::MeshRegister::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete<IMesh> (*m_model_Allocator.get(), it->second);
	}

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

void rlms::MeshRegister::RegisterAlias (IMESH_TYPE_ID type_id, std::string&& name) {
	name = MeshNameSanitizer::Sanitize (name);

	auto it = m_dict.find (name);
	if (it != m_dict.end()) {
		//alias already taken
		logger->tag (LogTags::Warn) << "Alias "<< name << " for id("<< type_id <<") already taken, by block of id(" << it->second << ")" << '\n';
		throw Exception ("Mesh Alias already Taken");
	}

	m_dict.emplace (std::make_pair (name, type_id));
}

void rlms::MeshRegister::imports () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->import ();
	}
}

void rlms::MeshRegister::optimises () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->optimise ();
	}
}

void rlms::MeshRegister::loads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->load ();
	}
}

void rlms::MeshRegister::unloads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->unload ();
	}
}

void rlms::MeshRegister::free () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete<IMesh> (*m_model_Allocator.get (), it->second);
	}
}

IMesh* rlms::MeshRegister::get (IMESH_TYPE_ID const& type_id) {
	return m_register[type_id];
}

IMesh* rlms::MeshRegister::get (std::string const& alias) {
	auto it = m_dict.find (alias);
	if (it == m_dict.end ()) {
		throw Exception ("Mesh not found");
	}
	IMESH_TYPE_ID const type_id = it->second;
	return m_register[type_id];
}

IMESH_TYPE_ID rlms::MeshRegister::getId (std::string const& alias) {
	auto it = m_dict.find (alias);
	if (it == m_dict.end ()) {
		throw Exception ("Mesh not found");

	}
	return it->second;
}

std::string rlms::MeshRegister::getAlias (IMESH_TYPE_ID const& type_id) {
	for (auto it = m_dict.begin(); it != m_dict.end(); it++) {
		if (it->second == type_id) return it->first;
	}
	throw Exception ("Mesh not found");
}
