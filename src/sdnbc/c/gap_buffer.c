#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/gap_buffer.h"

#define EXPORT __attribute__((visibility("default")))


typedef struct sdnb_gapBuffer_internal_s
{
    char *_bufStart;
    char *_bufEnd;
    char *_gapStart;
    char *_gapEnd;
    char *_data;
} sdnb_gapBuffer_internal_t;

void sdnb_gapBuffer_expand(sdnb_gapBuffer_t *buf);
void sdnb_gapBuffer_shrink(sdnb_gapBuffer_t *buf);

EXPORT
sdnb_gapBuffer_t *sdnb_gapBuffer_create(size_t size)
{
    sdnb_gapBuffer_t *buf = (sdnb_gapBuffer_t *)malloc(sizeof(sdnb_gapBuffer_t));
    sdnb_gapBuffer_internal_t *_internal = (sdnb_gapBuffer_internal_t *)malloc(sizeof(sdnb_gapBuffer_internal_t));
    buf->_internal = _internal;

    _internal->_bufStart = (char *)malloc(size);
    if (!_internal->_bufStart) {
        exit(1);
    }
    _internal->_bufEnd = _internal->_bufStart + size;
    _internal->_gapStart = _internal->_bufStart;
    _internal->_gapEnd = _internal->_bufEnd;
    _internal->_data = NULL;
    buf->length = 0;

    return buf;
}

EXPORT
void sdnb_gapBuffer_destroy(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_bufStart) {
        free(_internal->_bufStart);
        _internal->_bufStart = NULL;
        _internal->_bufEnd = NULL;
        _internal->_gapStart = NULL;
        _internal->_gapEnd = NULL;
    }

    if (_internal->_data) {
        free(_internal->_data);
        _internal->_data = NULL;
        buf->length = 0;
    }

    free(_internal);
    _internal = NULL;
    buf->_internal = NULL;
    free(buf);
    buf = NULL;
}

EXPORT
void sdnb_gapBuffer_insertChar(sdnb_gapBuffer_t *buf, const char c)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_gapStart == _internal->_gapEnd)
        sdnb_gapBuffer_expand(buf);

    *(_internal->_gapStart) = c;
    _internal->_gapStart++;
}

EXPORT
void sdnb_gapBuffer_insertString(sdnb_gapBuffer_t*buf, const char *str, size_t length)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    while ((_internal->_gapEnd - _internal->_gapStart) < length)
    {
        sdnb_gapBuffer_expand(buf);
    }

    memcpy(_internal->_gapStart, str, length);
    _internal->_gapStart += length;
}

EXPORT
char *sdnb_gapBuffer_remove(sdnb_gapBuffer_t *buf, size_t numChars, sdnb_direction_1d_t direction)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (direction == FL_1D_BACKWARD)
    {
        _internal->_gapStart -= numChars;
        return _internal->_gapStart; //caller must alloc mem
    }
    else
    {
        _internal->_gapEnd += numChars;
        return (_internal->_gapEnd - numChars); //caller must alloc mem
    }
}

EXPORT
const char *sdnb_gapBuffer_getData(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_data)
    {
        free(_internal->_data);
        _internal->_data = NULL;
    }

    buf->length = ((_internal->_gapStart - _internal->_bufStart) + \
                    (_internal->_bufEnd - _internal->_gapEnd));
    _internal->_data = (char *)malloc(buf->length);
    if (!_internal->_data)
        exit(1);

    memcpy(_internal->_data, \
            _internal->_bufStart, \
            (_internal->_gapStart - _internal->_bufStart));

    memcpy((_internal->_data + (_internal->_gapStart - _internal->_bufStart)), \
            _internal->_gapEnd, \
            (_internal->_bufEnd - _internal->_gapEnd));

    return _internal->_data;
}

EXPORT
void sdnb_gapBuffer_moveGap(sdnb_gapBuffer_t *buf, size_t numChars, sdnb_direction_1d_t direction)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (direction == FL_1D_BACKWARD && numChars <= (_internal->_gapStart - _internal->_bufStart)) {
        _internal->_gapStart -= numChars;
        _internal->_gapEnd -= numChars;
        memmove(_internal->_gapEnd, _internal->_gapStart, numChars);
    } else if (direction == FL_1D_FORWARD && numChars <= (_internal->_bufEnd - _internal->_gapEnd)) {
        memmove(_internal->_gapStart, _internal->_gapEnd, numChars);
        _internal->_gapStart += numChars;
        _internal->_gapEnd += numChars;
    } else {
        // you tried to move the gap out of bounds,
        // ya ding-a-ling
        // maybe should return an error/success value,
        // but it's not my job to prevent you from being a ding-a-ling
    }

    return;
}

void sdnb_gapBuffer_expand(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    size_t bufferLength = _internal->_bufEnd - _internal->_bufStart;
    size_t backBufferLength = _internal->_gapStart - _internal->_bufStart;
    size_t frontBufferLength = _internal->_bufEnd - _internal->_gapEnd;

    char *tempBufferStart = (char *)realloc(_internal->_bufStart, (bufferLength << 1));
    assert(tempBufferStart != NULL);
    _internal->_bufStart = tempBufferStart;
    _internal->_gapStart = _internal->_bufStart + backBufferLength;
    _internal->_gapEnd = _internal->_bufStart + bufferLength - frontBufferLength;
    _internal->_bufEnd = _internal->_bufStart + (bufferLength << 1);

    char *tempGapEnd = _internal->_bufEnd - frontBufferLength;
    void *memTest = memcpy(tempGapEnd, _internal->_gapEnd, frontBufferLength);
    assert(memTest != NULL);
    _internal->_gapEnd = tempGapEnd;
    tempGapEnd = NULL;
}

void sdnb_gapBuffer_shrink(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    size_t bufferLength = _internal->_bufEnd - _internal->_bufStart;
    size_t backBufferLength = _internal->_gapStart - _internal->_bufStart;
    size_t frontBufferLength = _internal->_bufEnd - _internal->_gapEnd;

    char *frontBuffer = (char *)malloc(frontBufferLength);
    void *memTest = memcpy(frontBuffer, _internal->_gapEnd, frontBufferLength);
    assert(memTest != NULL);
    char *tempBufferStart = (char *)realloc(_internal->_bufStart, (bufferLength >> 1));
    assert(tempBufferStart != NULL);
    _internal->_bufStart = tempBufferStart;
    tempBufferStart = NULL;
    _internal->_gapStart = _internal->_bufStart + backBufferLength;
    _internal->_bufEnd = _internal->_bufStart + (bufferLength >> 1);
    _internal->_gapEnd = _internal->_bufEnd - frontBufferLength;

    memTest = memcpy(_internal->_gapEnd, frontBuffer, frontBufferLength);
    assert(memTest != NULL);
    free(frontBuffer);
    frontBuffer = NULL;
}
