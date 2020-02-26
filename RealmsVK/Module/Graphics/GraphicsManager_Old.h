#pragma once
#include "../../Base/Logging/ILogged.h"
#include "../../Base/Allocators/Allocator.h"

#include <memory>

#include "MeshRegister.h"

namespace rlms {
	class GraphicsManagerImplOld;
	class GraphicsManagerOld {
	private:
		static std::unique_ptr<GraphicsManagerImplOld> instance;
	public:
		static std::shared_ptr<LoggerHandler> GetLogger ();

		static void Initialize (Allocator* const& alloc, size_t mesh_pool_size, std::shared_ptr<Logger> funnel = nullptr);
		static void Terminate ();

		static MeshRegister* GetRegister ();

		static void Register (IMESH_TYPE_ID const& type_id, std::string&& filename, std::string&& meshAlias = "");
		static IMesh* GetMesh (IMESH_TYPE_ID const& type_id);

		static void Load ();
		static void LoadModels ();
		static void LoadRenderer ();
		static void Draw ();
		static void Unload ();
	};
}

