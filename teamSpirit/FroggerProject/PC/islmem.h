#ifndef ISL_MEM_H
#define ISL_MEM_H

#include <memory.h>

#define FREE(x) free(x)
#define memoryAllocateZero(size, file, line) calloc((size), 1)
#define memoryShow()
#define memoryShowStats()


#endif