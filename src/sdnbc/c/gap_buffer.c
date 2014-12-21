#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/gap_buffer.h"

#define EXPORT __attribute__((visibility("default")))


typedef struct sdnb_gapBuffer_private_s
{
    char *_bufStart;
    char *_bufEnd;
    char *_gapStart;
    char *_gapEnd;
    char *_iter;
} sdnb_gapBuffer_private_t;

void sdnb_gapBuffer_expand(sdnb_gapBuffer_t *buf);
void sdnb_gapBuffer_shrink(sdnb_gapBuffer_t *buf);

EXPORT
sdnb_gapBuffer_t *sdnb_gapBuffer_create(size_t size)
{
    sdnb_gapBuffer_t *buf = (sdnb_gapBuffer_t *)malloc(sizeof(sdnb_gapBuffer_t));
    sdnb_gapBuffer_private_t *_private = (sdnb_gapBuffer_private_t *)malloc(sizeof(sdnb_gapBuffer_private_t));
    buf->_private = _private;

    _private->_bufStart = (char *)malloc(size);
    if (!_private->_bufStart) {
        exit(1);
    }
    _private->_bufEnd = _private->_bufStart + size;
    _private->_gapStart = _private->_bufStart;
    _private->_gapEnd = _private->_bufEnd;
    _private->_iter = NULL;
    buf->length = 0;
    buf->gapIndex = 0;

    return buf;
}

EXPORT
void sdnb_gapBuffer_destroy(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    if (_private->_bufStart) {
        free(_private->_bufStart);
        _private->_bufStart = NULL;
        _private->_bufEnd = NULL;
        _private->_gapStart = NULL;
        _private->_gapEnd = NULL;
        _private->_iter = NULL;
        buf->length = 0;
        buf->gapIndex = 0;
    }

    free(_private);
    _private = NULL;
    buf->_private = NULL;
    free(buf);
    buf = NULL;
}

EXPORT
void sdnb_gapBuffer_insertChar(sdnb_gapBuffer_t *buf, char c)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    if (_private->_gapStart == _private->_gapEnd)
        sdnb_gapBuffer_expand(buf);

    *(_private->_gapStart) = c;
    _private->_gapStart++;
    buf->gapIndex++;
    buf->length++;
}

EXPORT
void sdnb_gapBuffer_insertString(sdnb_gapBuffer_t*buf, const char *str, size_t length)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    while ((_private->_gapEnd - _private->_gapStart) < length)
    {
        sdnb_gapBuffer_expand(buf);
    }

    memcpy(_private->_gapStart, str, length);
    _private->_gapStart += length;
    buf->gapIndex += length;
    buf->length += length;
}

EXPORT
void sdnb_gapBuffer_remove(sdnb_gapBuffer_t *buf, int length)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    if (_private->_gapStart + length >= _private->_bufStart && \
        _private->_gapEnd + length <= _private->_bufEnd) {
        if (length < 0) {
            _private->_gapStart += length;
            buf->length += length;
            buf->gapIndex += length;
        } else {
            _private->_gapEnd += length;
            buf->length -= length;
        }
    }
}

EXPORT
void sdnb_gapBuffer_getData(sdnb_gapBuffer_t *buf, char *data, size_t from, size_t length)
{
    if (length == 0 || from >= buf->length) {
        return;
    }
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);
    size_t to = from + length - 1;
    if (to >= buf->length) {
        to = buf->length - 1;
    }

    if (to < buf->gapIndex) {
        memcpy(data, \
                _private->_bufStart + from, \
                (to - from + 1));
    } else if (from < buf->gapIndex && to >= buf->gapIndex) {
        size_t backLength = buf->gapIndex - from;
        size_t frontLength = (to - from + 1) - backLength;
        memcpy(data, \
                _private->_bufStart + from, \
                backLength);

        memcpy((data + backLength), \
                _private->_gapEnd, \
                frontLength);
    } else if (from >= buf->gapIndex) {
        memcpy(data, \
                _private->_gapEnd + (from - buf->gapIndex), \
              (to - from + 1));
    }

    data[buf->length]= '\0';
}

EXPORT
int sdnb_gapBuffer_moveGap(sdnb_gapBuffer_t *buf, int length)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    if (_private->_gapStart + length >= _private->_bufStart && \
        _private->_gapEnd + length <= _private->_bufEnd) {
        if (length < 0) {
            _private->_gapStart += length;
            _private->_gapEnd += length;
            memmove(_private->_gapEnd, _private->_gapStart, abs(length));
        } else {
            memmove(_private->_gapStart, _private->_gapEnd, length);
            _private->_gapStart += length;
            _private->_gapEnd += length;
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
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);
    char iterVal;
    if (index > buf->length) { //out of bounds
        iterVal = '\0';
    } else {
        if (index < buf->gapIndex) {
            _private->_iter = _private->_bufStart + index;
        } else {
            _private->_iter = _private->_gapEnd + (index - buf->gapIndex);
        }
        iterVal = *(_private->_iter);
    }

    return iterVal;
}

EXPORT
char sdnb_gapBuffer_iterNext(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);
    char iterVal;
    _private->_iter++;
    if (_private->_iter == _private->_bufEnd) { //out of bounds
        _private->_iter--;
        return '\0';
    }

    if (_private->_iter == _private->_gapStart) { 
        if (_private->_gapEnd < _private->_bufEnd) {//skip over gap
            _private->_iter = _private->_gapEnd;
        } else {
            _private->_iter--;
            return '\0';
        }
    }
    iterVal = *(_private->_iter);
    return iterVal;
}

EXPORT
char sdnb_gapBuffer_iterPrev(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);
    char iterVal;
    _private->_iter--;
    if (_private->_iter < _private->_bufStart) { //out of bounds
        _private->_iter++;
        return '\0';
    }

    if (_private->_iter < _private->_gapEnd &&
        _private->_iter >= _private->_gapStart) {
        if (_private->_gapStart > _private->_bufStart) { //skip over gap
            _private->_iter = _private->_gapStart - 1;
        } else {
            _private->_iter++;
            return '\0';
        }
    }
    iterVal = *(_private->_iter);
    return iterVal;
}

void sdnb_gapBuffer_expand(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    size_t bufferLength = _private->_bufEnd - _private->_bufStart;
    size_t backBufferLength = _private->_gapStart - _private->_bufStart;
    size_t frontBufferLength = _private->_bufEnd - _private->_gapEnd;

    char *tempBufferStart = (char *)realloc(_private->_bufStart, (bufferLength << 1));
    //assert(tempBufferStart != NULL);
    _private->_bufStart = tempBufferStart;
    _private->_gapStart = _private->_bufStart + backBufferLength;
    _private->_gapEnd = _private->_bufStart + bufferLength - frontBufferLength;
    _private->_bufEnd = _private->_bufStart + (bufferLength << 1);

    char *tempGapEnd = _private->_bufEnd - frontBufferLength;
    void *memTest = memcpy(tempGapEnd, _private->_gapEnd, frontBufferLength);
    //assert(memTest != NULL);
    _private->_gapEnd = tempGapEnd;
    tempGapEnd = NULL;
}

void sdnb_gapBuffer_shrink(sdnb_gapBuffer_t *buf)
{
    sdnb_gapBuffer_private_t *_private = ((sdnb_gapBuffer_private_t *)buf->_private);

    size_t bufferLength = _private->_bufEnd - _private->_bufStart;
    size_t backBufferLength = _private->_gapStart - _private->_bufStart;
    size_t frontBufferLength = _private->_bufEnd - _private->_gapEnd;

    char *frontBuffer = (char *)malloc(frontBufferLength);
    void *memTest = memcpy(frontBuffer, _private->_gapEnd, frontBufferLength);
    //assert(memTest != NULL);
    char *tempBufferStart = (char *)realloc(_private->_bufStart, (bufferLength >> 1));
    //assert(tempBufferStart != NULL);
    _private->_bufStart = tempBufferStart;
    tempBufferStart = NULL;
    _private->_gapStart = _private->_bufStart + backBufferLength;
    _private->_bufEnd = _private->_bufStart + (bufferLength >> 1);
    _private->_gapEnd = _private->_bufEnd - frontBufferLength;

    memTest = memcpy(_private->_gapEnd, frontBuffer, frontBufferLength);
    //assert(memTest != NULL);
    free(frontBuffer);
    frontBuffer = NULL;
}
