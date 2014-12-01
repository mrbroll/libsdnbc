#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ptr.h"

#define EXPORT __attribute__((visibility("hidden")))


//private
typedef struct sdnb_ptr_private_s
{
    size_t dataSize;
    size_t refCount;
} sdnb_ptr_private_t;

EXPORT
sdnb_ptr_t *sdnb_ptr_create(void *data, size_t dataSize)
{
    sdnb_ptr_t *ptr = (sdnb_ptr_t *)malloc(sizeof(sdnb_ptr_t));
    ptr->data = malloc(dataSize);
    if (!ptr->data) {
        fprintf(stderr, "Unable to allocate memory for ptr data\n");
        exit(1);
    }
    void *noErr = memcpy(ptr->data, data, dataSize);
    if (!noErr) {
        fprintf(stderr, "Unable to create sdnb_ptr, exiting\n");
        exit(1);
    } 
    sdnb_ptr_private_t *_private = (sdnb_ptr_private_t *)malloc(sizeof(sdnb_ptr_private_t));
    if (!_private) {
        exit(1);
    }
    _private->dataSize = dataSize;
    _private->refCount = 1;
    ptr->_private = _private;
    return ptr;
}

EXPORT
void sdnb_ptr_destroy(sdnb_ptr_t *ptr)
{
    free(ptr->_private);
    free(ptr->data);
    free(ptr);
}

EXPORT
sdnb_ptr_t *sdnb_ptr_incref(sdnb_ptr_t *ptr)
{
    sdnb_ptr_private_t *_private = ((sdnb_ptr_private_t *)ptr->_private);
    _private->refCount++;
    return ptr;
}

EXPORT
void sdnb_ptr_decref(sdnb_ptr_t *ptr)
{
    sdnb_ptr_private_t *_private = ((sdnb_ptr_private_t *)ptr->_private);
    _private->refCount--;
    if (_private->refCount == 0) {
        sdnb_ptr_destroy(ptr);
    }
}
