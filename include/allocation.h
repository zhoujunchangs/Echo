#pragma once

#ifndef __ALLOCATION_H_
#define __ALLOCATION_H_

#include "util.h"
#include <memory>

namespace Allocation {

	using MemoryBlock = void*;
	using MemoryBlockSize = long long;

	static inline MemoryBlock allocBlock(const MemoryBlockSize& size) {
		if (size > 0) {
			MemoryBlock ptr = (MemoryBlock)(new char[size]);
			memset(ptr, 0, size);
			return ptr;
		}
		else return MemoryBlock();
	}

	
	static inline void deallocBlock(MemoryBlock ptr) {
		delete[] ptr;
	}

}

#endif // !__ALLOCATION_H_
