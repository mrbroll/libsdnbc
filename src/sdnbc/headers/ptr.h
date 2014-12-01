#ifndef _SDNB_PTR_H_
#define _SDNB_PTR_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sdnb_ptr_s
{
    void *data;
    void *_private;
} sdnb_ptr_t;


sdnb_ptr_t *sdnb_ptr_create(void *data, size_t dataSize);
void sdnb_ptr_destroy(sdnb_ptr_t* ptr);
sdnb_ptr_t *sdnb_ptr_incref(sdnb_ptr_t *ptr);
void sdnb_ptr_decref(sdnb_ptr_t *ptr);

#ifdef __cplusplus
}
#endif

#endif
