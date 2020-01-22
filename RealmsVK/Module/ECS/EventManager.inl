
template<class E> inline bool EventManager::CreateEvent () {
	return instance->createEvent<E> ();
}

template<class E> inline E* EventManager::GetEvent () {
	return instance->getEvent<E> ();
}

template<class E> inline bool EventManager::HasEvent () {
	return instance->hasEvent<E> ();
}

template<class E> inline bool EventManagerImpl::createEvent () {
	logger->tag (LogTags::Debug) << "Creating " << typeid(E).name () << "." << '\n';

	auto it = _events.find (&typeid(E));

   //Event doesn't exists
	if (it != _events.end ()) {
		logger->tag (LogTags::Error) << typeid(E).name () << " already exists." << '\n';
		EventManager::n_errors++;
		return false;
	}

	//Valid
	E* new_event = new (_event_Allocator->allocate (sizeof (E), __alignof(E))) E ();
	_events.insert (std::pair<const std::type_info*, IEvent*> (&typeid(E), new_event));
	return true;
}

template<class E> inline E* EventManagerImpl::getEvent () {
	logger->tag (LogTags::Debug) << "Getting " << typeid(E).name () << "." << '\n';

	auto it = _events.find (&typeid(E));

   //Event does exists
	if (it == _events.end ()) {
		logger->tag (LogTags::Error) << typeid(E).name () << " doesn't exists." << '\n';
		EventManager::n_errors++;
		return nullptr;
	}

	return static_cast<E*>(it->second);
}

template<class E> inline bool EventManagerImpl::hasEvent () {
	return _events.find (&typeid(E)) != _events.end ();
}
