#ifndef ISL_MEM_H
#define ISL_MEM_H

#include <memory.h>

#define MALLOC(x) malloc(x)
#define FREE(x) free(x),(x)=NULL
#define memoryAllocateZero(size, file, line) calloc((size), 1)
#define memoryShow()
#define memoryShowStats()


#endif