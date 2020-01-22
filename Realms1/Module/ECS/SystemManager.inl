
template<class S>
inline bool SystemManager::CreateSystem () {
	return instance->createSystem<S> ();
}

template<class S>
inline S* SystemManager::GetSystem () {
	return instance->getSystem<S> ();
}

template<class S>
inline bool SystemManager::HasSystem () {
	return instance->hasSystem<S> ();
}

template<class S>
inline void SystemManager::DestroySystem () {
	instance->destroySystem<S> ();
}

/////////

template<class S> inline bool SystemManagerImpl::createSystem () {
	logger->tag (LogTags::Debug) << "creating " << typeid(S).name () << "." << '\n';

	//template is a system
	if (!std::is_base_of<ISystem, S>::value && typeid(S) != typeid(ISystem)) {
		logger->tag (LogTags::Error) << "System must inherit from ISystem !" << '\n';
		SystemManager::n_errors++;
		return false;
	}

	auto it = _systems.find (&typeid(S));

	//System type exists
	if (it != _systems.end ()) {
		logger->tag (LogTags::Error) << "System already exist !" << '\n';
		SystemManager::n_errors++;
		return false;
	}

	//Valid
	S* new_system = new (m_object_Allocator->allocate (sizeof (S), __alignof(S))) S ();
	_systems.insert (std::pair<const std::type_info*, ISystem*> (&typeid(S), static_cast<ISystem*>(new_system)));
	return true;
}

template<class S> inline S* SystemManagerImpl::getSystem () {
	logger->tag (LogTags::Debug) << "getting " << typeid(S).name () << "." << '\n';

	//template is a system
	if (!std::is_base_of<ISystem, S>::value) {
		logger->tag (LogTags::Error) << "Systems must inherit from ISystem !" << '\n';
		SystemManager::n_errors++;
		return nullptr;
	}

	auto it = _systems.find (&typeid(S));

	//System type exists
	if (it == _systems.end ()) {
		logger->tag (LogTags::Error) << "System doesn't exist !" << '\n';
		SystemManager::n_errors++;
		return nullptr;
	}

	return static_cast<S*>(it->second);
}

template<class S> inline bool SystemManagerImpl::hasSystem () {
	return _systems.find (&typeid(S)) != _systems.end ();
}

template<class S> inline void SystemManagerImpl::destroySystem () {
	logger->tag (LogTags::Debug) << "Destroying " << typeid(S).name () << "." << '\n';

	//template is a system
	if (!std::is_base_of<ISystem, S>::value) {
		logger->tag (LogTags::Error) << "Systems must inherit from ISystem !" << '\n';
		SystemManager::n_errors++;
		return;
	}

	auto it = _systems.find (&typeid(S));

	if (it == _systems.end ()) {
		logger->tag (LogTags::Error) << "System doesn't exist !" << '\n';
		SystemManager::n_errors++;
		return;
	}

	S* sys = static_cast<S*>(it->second);

	sys->~S ();
	m_object_Allocator->deallocate (sys);
	_systems.erase (it->first);
}
