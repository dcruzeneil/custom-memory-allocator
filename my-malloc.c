#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct allocation{
    struct allocation *next;
    int status; //0 = free, 1 = taken
} allocation;

/* Function Prototypes */
size_t malloc_usable_size(void*);
size_t padding(size_t);
void* calculateAddress(allocation*, size_t);
void* updateMetaData(allocation*, size_t, int);

#define BRK_MEMORY_SIZE 4096
#define byteBoundary 16

static void* beginningAddress = NULL;
static void* heapEnd = NULL;

void* malloc(size_t size){
    void* availableAddress; 
    int sizeAvailable;
    size_t heapIncreaseSize; 
    allocation* metaData = beginningAddress; 

    size = padding(size) + byteBoundary;

    // FIRST TIME: need to do a system call to request memory from the heap
    if(beginningAddress == NULL){
        heapIncreaseSize = (size > BRK_MEMORY_SIZE + byteBoundary) ? size : BRK_MEMORY_SIZE + byteBoundary;
        beginningAddress = sbrk(heapIncreaseSize); 
        if(beginningAddress == (void*)-1){
            return NULL;
        }
        // updating the metadata 
        metaData = beginningAddress;
        // meta data should be updated by the value user asked for i.e. size
        availableAddress = updateMetaData(metaData, size, 1);
        // ending address should be increased by value that was actually passed into sbrk i.e. heapIncreaseSize
        heapEnd = (void*) ((uintptr_t) metaData + heapIncreaseSize);
        return availableAddress;
    }

    // cycling through available memory
    while(metaData->next != NULL){
        if(metaData->status == 0){
            sizeAvailable = (uintptr_t) metaData->next - (uintptr_t) metaData;
            if(sizeAvailable >= size){
                availableAddress = updateMetaData(metaData, size, 0);
                return availableAddress;
            }
        }
        metaData = metaData->next;
        
        // added check to see if the last linked list
        // is actually pointing to the heapEnd - beyond our heap area
        if((void*) metaData == heapEnd){
            break;
        }
    }

    // could not find memory cycling through 
    sizeAvailable = (uintptr_t) heapEnd - (uintptr_t) metaData;
    
    if(sizeAvailable > size){
        availableAddress = updateMetaData(metaData, size, 1);
    }   
    else if(sizeAvailable <= size){
        heapIncreaseSize = (size > BRK_MEMORY_SIZE + byteBoundary) ? size : BRK_MEMORY_SIZE + byteBoundary;
        availableAddress = sbrk(heapIncreaseSize);
        if(availableAddress == (void*)-1){
            return NULL;
        }
        // the metadata should be updated by the value that user asked for i.e. size
        availableAddress = updateMetaData(metaData, size, 1);
        // ending address should be increased by value that was actually passed into sbrk i.e. heapIncreaseSize
        heapEnd = calculateAddress(heapEnd, heapIncreaseSize);
    }
    
    return availableAddress;
}

void free(void* freeAddress){
    if(freeAddress != NULL){
        allocation* metaData = (allocation*) ((uintptr_t) freeAddress - byteBoundary);
        metaData->status = 0;
    }
}

void* calloc(size_t number, size_t size){
    void* availableAddress;

    size_t requiredMemory = number * size;
    // checking for integer overflow
    if(number > 0 && requiredMemory/number != size){
        return NULL;
    }

    availableAddress = malloc(requiredMemory);
    if(availableAddress == NULL){
        return NULL;
    }

    availableAddress = memset(availableAddress, 0, malloc_usable_size(availableAddress));
    return availableAddress;
}

void* realloc(void* a, size_t size){
    void* availableAddress = NULL;
    size_t availableSpace; 

    // error checking - is it actually pointing to an existing block of memory? 
    if(size == 0){
        free(a);
        return NULL;
    }
    if(a == NULL && size != 0){
        availableAddress = malloc(size);
        return availableAddress;
    }

    availableAddress = a;
    availableSpace = malloc_usable_size(a);

    // if size requested for is larger than the available space
    if(availableSpace < size){
        availableAddress = malloc(size);
        if(availableAddress == NULL){
            return NULL;
        }
        memcpy(availableAddress, a, availableSpace);
        free(a);
    }
    
    return availableAddress;
}

size_t malloc_usable_size(void* a){
    size_t availableSpace;
    allocation* metaData = (allocation*)((uintptr_t) a - byteBoundary);
    availableSpace = (uintptr_t) metaData->next - (uintptr_t) a;
    return availableSpace;
}

void* updateMetaData(allocation* a, size_t size, int nextPointerUpdate){
    void* returnAddress;
    // updating the fields of the metadata struct 
    a->status = 1;
    if(nextPointerUpdate == 1){
        a->next = calculateAddress(a, size);
    }
    // calculating and returning the available address
    returnAddress = calculateAddress(a, byteBoundary);
    return returnAddress;
}

size_t padding(size_t size){
    double tempCalculation;
    tempCalculation = ((double) size / (double) byteBoundary) + 1;
    size = byteBoundary * (size_t) tempCalculation;
    return size;
}

void* calculateAddress(allocation* a, size_t size){
    uintptr_t operation;
    void* newPointer;
    operation = (uintptr_t) a;
    operation += size;
    newPointer = (void*) operation;
    return newPointer;
}