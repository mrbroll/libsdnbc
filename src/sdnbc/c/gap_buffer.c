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
    char *_iter;
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
    _internal->_iter = NULL;
    buf->length = 0;
    buf->gapIndex = 0;

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
        _internal->_iter = NULL;
        buf->length = 0;
        buf->gapIndex = 0;
    }

    free(_internal);
    _internal = NULL;
    buf->_internal = NULL;
    free(buf);
    buf = NULL;
}

EXPORT
void sdnb_gapBuffer_insertChar(sdnb_gapBuffer_t *buf, char c)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_gapStart == _internal->_gapEnd)
        sdnb_gapBuffer_expand(buf);

    *(_internal->_gapStart) = c;
    _internal->_gapStart++;
    buf->gapIndex++;
    buf->length++;
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
    buf->gapIndex += length;
    buf->length += length;
}

EXPORT
void sdnb_gapBuffer_remove(sdnb_gapBuffer_t *buf, int length)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_gapStart + length >= _internal->_bufStart && \
        _internal->_gapEnd + length <= _internal->_bufEnd) {
        if (length < 0) {
            _internal->_gapStart += length;
            buf->length += length;
            buf->gapIndex += length;
        } else {
            _internal->_gapEnd += length;
            buf->length -= length;
        }
    }
}

EXPORT
void sdnb_gapBuffer_getData(sdnb_gapBuffer_t *buf, char *data, size_t from, size_t to)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (from > to || to > buf->length) {
        return;
    }

    if (to < buf->gapIndex) {
        memcpy(data, \
                _internal->_bufStart + from, \
                (to - from + 1));
    } else if (from < buf->gapIndex && to >= buf->gapIndex) {
        size_t backLength = buf->gapIndex - from;
        size_t frontLength = (to - from + 1) - backLength;
        memcpy(data, \
                _internal->_bufStart + from, \
                backLength);

        memcpy((data + backLength), \
                _internal->_gapEnd, \
                frontLength);
    } else if (from >= buf->gapIndex) {
        memcpy(data, \
                _internal->_gapEnd + (from - buf->gapIndex), \
              (to - from + 1));
    }

    data[buf->length]= '\0';
}

EXPORT
int sdnb_gapBuffer_moveGap(sdnb_gapBuffer_t *buf, int length)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    if (_internal->_gapStart + length >= _internal->_bufStart && \
        _internal->_gapEnd + length <= _internal->_bufEnd) {
        if (length < 0) {
            _internal->_gapStart += length;
            _internal->_gapEnd += length;
            memmove(_internal->_gapEnd, _internal->_gapStart, abs(length));
        } else {
            memmove(_internal->_gapStart, _internal->_gapEnd, length);
            _internal->_gapStart += length;
            _internal->_gapEnd += length;
        }
        buf->gapIndex += length;
        return 0;
    } else {
        return 1;
    }
}

EXPORT
char sdnb_gapBuffer_iterSet(sdnb_gapBuffer_t *buf, size_t index)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);
    char iterVal;
    if (index > buf->length) { //out of bounds
        iterVal = '\0';
    } else {
        if (index < buf->gapIndex) {
            _internal->_iter = _internal->_bufStart + index;
        } else {
            _internal->_iter = _internal->_gapEnd + (index - buf->gapIndex);
        }
        iterVal = *(_internal->_iter);
    }

    return iterVal;
}

EXPORT
char sdnb_gapBuffer_iterNext(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);
    char iterVal;
    if (_internal->_iter >= _internal->_bufEnd) { //trying to iterate past end
        iterVal = '\0';
    } else {
        _internal->_iter++;
        iterVal = *(_internal->_iter);
    }
    return iterVal;
}

EXPORT
char sdnb_gapBuffer_iterPrev(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);
    char iterVal;
    if (_internal->_iter == _internal->_bufStart) {
        iterVal = '\0';
    } else {
        _internal->_iter--;
        iterVal = *(_internal->_iter);
    }
    return iterVal;
}

void sdnb_gapBuffer_expand(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_internal_t *_internal = ((sdnb_gapBuffer_internal_t *)buf->_internal);

    size_t bufferLength = _internal->_bufEnd - _internal->_bufStart;
    size_t backBufferLength = _internal->_gapStart - _internal->_bufStart;
    size_t frontBufferLength = _internal->_bufEnd - _internal->_gapEnd;

    char *tempBufferStart = (char *)realloc(_internal->_bufStart, (bufferLength << 1));
    //assert(tempBufferStart != NULL);
    _internal->_bufStart = tempBufferStart;
    _internal->_gapStart = _internal->_bufStart + backBufferLength;
    _internal->_gapEnd = _internal->_bufStart + bufferLength - frontBufferLength;
    _internal->_bufEnd = _internal->_bufStart + (bufferLength << 1);

    char *tempGapEnd = _internal->_bufEnd - frontBufferLength;
    void *memTest = memcpy(tempGapEnd, _internal->_gapEnd, frontBufferLength);
    //assert(memTest != NULL);
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
    //assert(memTest != NULL);
    char *tempBufferStart = (char *)realloc(_internal->_bufStart, (bufferLength >> 1));
    //assert(tempBufferStart != NULL);
    _internal->_bufStart = tempBufferStart;
    tempBufferStart = NULL;
    _internal->_gapStart = _internal->_bufStart + backBufferLength;
    _internal->_bufEnd = _internal->_bufStart + (bufferLength >> 1);
    _internal->_gapEnd = _internal->_bufEnd - frontBufferLength;

    memTest = memcpy(_internal->_gapEnd, frontBuffer, frontBufferLength);
    //assert(memTest != NULL);
    free(frontBuffer);
    frontBuffer = NULL;
}
