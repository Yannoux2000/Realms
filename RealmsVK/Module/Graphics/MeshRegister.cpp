#include "MeshRegister.h"

#include "../../Base/Exception.h"
#include "../../Base/Allocators/IAllocator.inl"

#include "MeshNameSanitizer.h"

using namespace rlms;

void MeshRegister::start (IAllocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel) {
	startLogger (funnel);
	logger->tag (LogTags::None) << "Initializing !" << '\n';

	m_model_Allocator = std::unique_ptr<alloc_type> (new alloc_type (alloc->allocate (mesh_pool_size), mesh_pool_size));

	logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
}

void MeshRegister::stop () {
	logger->tag (LogTags::None) << "Stopping !" << '\n';

	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete (*m_model_Allocator.get(), it->second);
	}

	logger->tag (LogTags::None) << "Stopped correctly !" << '\n';
}

void MeshRegister::RegisterAlias (IMODEL_TYPE_ID type_id, std::string&& name) {
	name = MeshNameSanitizer::Sanitize (name);

	auto it = m_dict.find (name);
	if (it != m_dict.end()) {
		//alias already taken
		logger->tag (LogTags::Warn) << "Alias "<< name << " for id("<< type_id <<") already taken, by block of id(" << it->second << ")" << '\n';
		throw Exception ("Mesh Alias already Taken");
	}

	m_dict.emplace (std::make_pair (name, type_id));
}

void MeshRegister::imports () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->import ();
	}
}

void MeshRegister::optimises () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->optimise ();
	}
}

void MeshRegister::loads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->load ();
	}
}

void MeshRegister::unloads () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		it->second->unload ();
	}
}

void MeshRegister::free () {
	for (auto it = m_register.begin (); it != m_register.end (); it++) {
		allocator::deallocateDelete (*m_model_Allocator.get (), it->second);
	}
}

IMesh* MeshRegister::get (IMODEL_TYPE_ID const& type_id) {
	return m_register[type_id];
}

IMesh* MeshRegister::get (std::string && alias) {
	auto it = m_dict.find (alias);
	if (it == m_dict.end ()) {
		throw Exception ("Mesh not found");
	}
	IMODEL_TYPE_ID const type_id = it->second;
	return m_register[type_id];
}
