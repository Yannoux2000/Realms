#pragma once

#include "../../Base/Allocators/FreeListAllocator.h"
#include "../../Base/Logging/ILogged.h"

#include "Chunk.h"

#include <vector>

namespace rlms {
	class ChunkManager : public ILogged {
	private:
	////////////////////////////////////////////////////////////
	///
	/// \brief Chunks will have multiple status:
	///	         non existing : the chunk as never been created
	///          - Generate
	///        - unimported : the chunk is in on the hard drive
	///          - Create
	///        - unimported : the chunk is in on the hard drive
	///        - loaded : the chunk exist in the heap and can be used
	///
	///        - updated : the chunk has been processed
	///        - updated : the chunk has been processed
	///        - render : the chunk will be rendered
	///        - saving : a copy of the chunk will be placed on the hard drive
	///
	///        - unloaded : the chunk has been deleted from the heap and must be saved
	///        
	///
	///
	///
	///
	///
	///
	///
	///
	///
	///
	////////////////////////////////////////////////////////////

		std::vector<Chunk*> m_activeChunks;

		std::unique_ptr<FreeListAllocator> m_chunk_allocator;
	public:

		std::string getLogName () override {
			return "ChunkManager";
		};

		ChunkManager () : m_activeChunks(), m_chunk_allocator() {};
		ChunkManager (Allocator* const& alloc, size_t chunk_pool_size, std::shared_ptr<Logger> funnel = nullptr) {

			startLogger (funnel);
			logger->tag (LogTags::None) << "Initializing !" << '\n';

			m_chunk_allocator = std::unique_ptr<FreeListAllocator> (new FreeListAllocator (alloc->allocate (chunk_pool_size), chunk_pool_size));

			logger->tag (LogTags::None) << "Initialized correctly !" << '\n';
		};
		void createChunk () {

		}
	};
}

