#pragma once
#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include <memory>

#include "MeshRegister.h"

namespace rlms {
	class GraphicsManagerImpl;
	class GraphicsManager {
	private:
		static std::unique_ptr<GraphicsManagerImpl> instance;
	public:
		static std::shared_ptr<LoggerHandler> GetLogger ();

		static void Initialize (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static MeshRegister* GetRegister ();

		static void Register (IMODEL_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias = "");
		static IMesh* GetMesh (IMODEL_TYPE_ID const& type_id);

		static void Load ();
		static void LoadModels ();
		static void LoadRenderer ();
		static void Draw ();
		static void Unload ();
	};
}

