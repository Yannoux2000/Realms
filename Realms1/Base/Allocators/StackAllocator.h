#include "Allocator.h" 

class StackAllocator : public Allocator {
public:

	StackAllocator (void* start, size_t size);
	~StackAllocator ();

	void* allocate (size_t size, uint8_t alignment) override;
	void deallocate (void*&& p) override;

private:
	//Prevent copies because it might cause errors 
	StackAllocator (const StackAllocator&);
	StackAllocator& operator=(const StackAllocator&) = delete;

	struct AllocationHeader {
		uint8_t adjustment;
	};
	void* _current_pos;
};