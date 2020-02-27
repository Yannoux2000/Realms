#pragma once

namespace rlms {

	struct GameCoreSettings {
		unsigned long long game_mem_alloc_size;

		unsigned long long entity_mem_alloc_size;
		unsigned long long component_mem_alloc_size;
		unsigned long long system_mem_alloc_size;
		unsigned long long event_mem_alloc_size;
		unsigned long long world_mem_alloc_size;

		double atomic_tick_time;
	};

	class GameCoreSettingsImporter {
	private:
		static GameCoreSettings stgs;
	public:
		static void import ();
		static GameCoreSettings& get ();
	};

}