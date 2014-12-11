#include <stdlib.h>

#ifndef _SDNB_GAP_BUFFER_H_
#define _SDNB_GAP_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sdnb_gapBuffer_s
{
    size_t length;
    size_t gapIndex;
    void *_private; //private - don't touch!
} sdnb_gapBuffer_t;

sdnb_gapBuffer_t *sdnb_gapBuffer_create(size_t size);
void sdnb_gapBuffer_insertChar(sdnb_gapBuffer_t *buf, char c);
void sdnb_gapBuffer_insertString(sdnb_gapBuffer_t *buf, const char *c, size_t length);
void sdnb_gapBuffer_remove(sdnb_gapBuffer_t *buf, int length);
int sdnb_gapBuffer_moveGap(sdnb_gapBuffer_t *buf, int length);
void sdnb_gapBuffer_getData(sdnb_gapBuffer_t *buf, char *data, size_t from, size_t length);
void sdnb_gapBuffer_destroy(sdnb_gapBuffer_t *buf);
char sdnb_gapBuffer_iterSet(sdnb_gapBuffer_t *buf, size_t index);
char sdnb_gapBuffer_iterNext(sdnb_gapBuffer_t *buf);
char sdnb_gapBuffer_iterPrev(sdnb_gapBuffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif
