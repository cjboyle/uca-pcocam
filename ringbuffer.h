#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <glib.h>

#include "stackbuffer.h"

struct _RingBuffer;
typedef struct _RingBuffer RingBuffer;

/* Allocates a new RingBuffer with the given capacity. */
RingBuffer *rbuf_sized_new(guint capacity);
// RingBuffer *rbuf_sized_new(gsize n_blocks, guint block_size);

/* Frees a RingBuffer including its array contents. */
void rbuf_free(RingBuffer *rbuf);

/* Resets a RingBuffer's read/write pointers, essentially resetting access to the underlying array. */
void rbuf_reset(RingBuffer *rbuf);

/* Returns TRUE if data can be read from a RingBuffer, and FALSE otherwise. */
gboolean rbuf_read_available(RingBuffer *rbuf);

/* Reads an element from a RingBuffer then advances its read pointer.
   Returns NULL if the read pointer is the same as the write pointer (i.e. the user is up-to-date). */
gpointer rbuf_read(RingBuffer *rbuf);

/* Writes an element to a RingBuffer then advances its write pointer.
   May overwrite existing array elements. */
void rbuf_write(RingBuffer *rbuf, gpointer data);

gpointer rbuf_write_replace(RingBuffer *rbuf, gpointer data);

// gpointer rbuf_get_write_pointer(RingBuffer *rbuf);

void rbuf_advance(RingBuffer *rbuf);

/* Returns the number of elements in the RingBuffer. */
guint rbuf_length(RingBuffer *rbuf);

/* Returns a pointer to the underlying array, sorted chronologically. */
gpointer rbuf_to_array(RingBuffer *rbuf);

/* Convert a RingBuffer to a StackBuffer, sorted chronologically.
   This is useful when a RingBuffer is used for writing only, but want to read data from newest to oldest available data. */
StackBuffer *rbuf_to_sbuf(RingBuffer *rbuf);

#endif /*_RINGBUFFER_H_*/