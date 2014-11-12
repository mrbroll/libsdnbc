#include <stdlib.h>

#ifndef _FL_DICTIONARY_H_
#define _FL_DICTIONARY_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sdnb_dictionary_s
{
    size_t numEntries;
    void *entries;  //private, don't touch
} sdnb_dictionary_t;

sdnb_dictionary_t *sdnb_dictionary_create(void);

//user needs to cast to appropriate data type
void *sdnb_dictionary_get(sdnb_dictionary_t *dict, const char *key);

//return 1 if key exists or if we can't allocate mem - might create actual error codes at some point
int sdnb_dictionary_set(sdnb_dictionary_t *dict, char *key, void *data, size_t dataSize);
void sdnb_dictionary_remove(sdnb_dictionary_t *dict, char *key);
void sdnb_dictionary_destroy(sdnb_dictionary_t *dict);

#ifdef __cplusplus
}
#endif

#endif
