#ifndef _STACKBUFFER_H_
#define _STACKBUFFER_H_

#include <glib.h>

struct _StackBuffer;
typedef struct _StackBuffer StackBuffer;

/* Allocates a new StackBuffer. */
StackBuffer *sbuf_new();

/* Allocates a new StackBuffer with preallocated array space. */
StackBuffer *sbuf_sized_new(guint initial_capacity);

/* Frees a StackBuffer including its array contents. */
void sbuf_free(StackBuffer *sbuf);

/* Adds an element to the top of a StackBuffer. */
void sbuf_push(StackBuffer *sbuf, gpointer ptr);

/* Returns the element at the top of a StackBuffer. Returns NULL if the StackBuffer is empty. */
gpointer sbuf_peek(StackBuffer *sbuf);

/* Returns the element at an index within a StackBuffer. Returns NULL if the StackBuffer is empty. */
gpointer sbuf_peek_index(StackBuffer *sbuf, guint index);

/* Takes the element at the top of a StackBuffer. Returns NULL if the StackBuffer is empty. */
gpointer sbuf_pop(StackBuffer *sbuf);

/* Takes the element at the bottom of a StackBuffer. Returns NULL if the StackBuffer is empty. */
gpointer sbuf_shift(StackBuffer *sbuf);

/* Returns the number of elements in the StackBuffer. */
guint sbuf_length(StackBuffer *sbuf);

/* Returns TRUE if the StackBuffer contains no elements, otherwise FALSE. */
gboolean sbuf_is_empty(StackBuffer *sbuf);

/* Returns a pointer to the underlying array. */
gpointer sbuf_to_array(StackBuffer *sbuf);

#endif /*_STACKBUFFER_H_*/