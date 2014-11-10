#include <stdlib.h>

#ifndef _FL_DICTIONARY_H_
#define _FL_DICTIONARY_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fl_dictionary_s
{
    size_t numEntries;
    void *entries;  //private, don't touch
} fl_dictionary_t;

fl_dictionary_t *fl_dictionary_create(void);

//user needs to cast to appropriate data type
void *fl_dictionary_get(fl_dictionary_t *dict, const char *key);

//return 1 if key exists or if we can't allocate mem - might create actual error codes at some point
int fl_dictionary_set(fl_dictionary_t *dict, char *key, void *data, size_t dataSize);
void fl_dictionary_remove(fl_dictionary_t *dict, char *key);
void fl_dictionary_destroy(fl_dictionary_t *dict);

#ifdef __cplusplus
}
#endif

#endif
