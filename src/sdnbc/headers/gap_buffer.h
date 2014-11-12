#include <stdlib.h>

#ifndef _SDNB_GAP_BUFFER_H_
#define _SDNB_GAP_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum sdnb_direction_1d_e
{
    FL_1D_FORWARD,
    FL_1D_BACKWARD
} sdnb_direction_1d_t;

typedef struct sdnb_gapBuffer_s
{
    size_t length;
    void *_internal; //private - don't touch!
} sdnb_gapBuffer_t;

sdnb_gapBuffer_t *sdnb_gapBuffer_create(size_t size);
void sdnb_gapBuffer_insertChar(sdnb_gapBuffer_t *buf, const char c);
void sdnb_gapBuffer_insertString(sdnb_gapBuffer_t *buf, const char *c, size_t length);
char *sdnb_gapBuffer_remove(sdnb_gapBuffer_t *buf, size_t numChars, sdnb_direction_1d_t direction);
void sdnb_gapBuffer_moveGap(sdnb_gapBuffer_t *buf, size_t numChars, sdnb_direction_1d_t direction);
const char *sdnb_gapBuffer_getData(sdnb_gapBuffer_t *buf);
void sdnb_gapBuffer_destroy(sdnb_gapBuffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif
