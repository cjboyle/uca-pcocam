#include <glib.h>
#include "stackbuffer.h"

struct _StackBuffer {
    GPtrArray *array;
    gint64 tail;
};

StackBuffer *sbuf_new()
{
    StackBuffer *sbuf = g_malloc0(sizeof(StackBuffer));
    sbuf->array = g_ptr_array_new();
    sbuf->tail = -1;
    return sbuf;
}

StackBuffer *sbuf_sized_new(guint initial_capacity)
{
    StackBuffer *sbuf = g_malloc0(sizeof(StackBuffer));
    sbuf->array = g_ptr_array_sized_new(initial_capacity);
    sbuf->tail = -1;
    return sbuf;
}

void sbuf_free(StackBuffer *sbuf)
{
    g_ptr_array_free(sbuf->array, TRUE);
    g_free(sbuf);
}

void sbuf_push(StackBuffer *sbuf, gpointer ptr)
{
    g_ptr_array_add(sbuf->array, ptr);
    sbuf->tail++;
}

gpointer sbuf_peek(StackBuffer *sbuf)
{
    return sbuf_peek_index(sbuf, sbuf->tail);
}

gpointer sbuf_peek_index(StackBuffer *sbuf, guint index)
{
    if (sbuf_is_empty(sbuf))
        return NULL;
    return g_ptr_array_index(sbuf->array, index);
}

gpointer sbuf_pop(StackBuffer *sbuf)
{
    if (sbuf_is_empty(sbuf))
        return NULL;
    return g_ptr_array_remove_index(sbuf->array, sbuf->tail--);
}

gpointer sbuf_shift(StackBuffer *sbuf)
{
    if (sbuf_is_empty(sbuf))
        return NULL;
    return g_ptr_array_remove_index(sbuf->array, 0);
}

guint sbuf_length(StackBuffer *sbuf)
{
    return sbuf->tail + 1;
}

gboolean sbuf_is_empty(StackBuffer *sbuf)
{
    return sbuf_length(sbuf) == 0;
}

gpointer sbuf_to_array(StackBuffer *sbuf)
{
    return sbuf->array->pdata;
}