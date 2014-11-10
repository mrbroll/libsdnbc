#include <stdlib.h>

#ifndef _FL_GAP_BUFFER_H_
#define _FL_GAP_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum fl_direction_1d_e
{
    FL_1D_FORWARD,
    FL_1D_BACKWARD
} fl_direction_1d_t;

typedef struct fl_gapBuffer_s
{
    size_t length;
    void *_internal; //private - don't touch!
} fl_gapBuffer_t;

fl_gapBuffer_t *fl_gapBuffer_create(size_t size);
void fl_gapBuffer_insertChar(fl_gapBuffer_t *buf, const char c);
void fl_gapBuffer_insertString(fl_gapBuffer_t *buf, const char *c, size_t length);
char *fl_gapBuffer_remove(fl_gapBuffer_t *buf, size_t numChars, fl_direction_1d_t direction);
void fl_gapBuffer_moveGap(fl_gapBuffer_t *buf, size_t numChars, fl_direction_1d_t direction);
const char *fl_gapBuffer_getData(fl_gapBuffer_t *buf);
void fl_gapBuffer_destroy(fl_gapBuffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif
