#include <math.h>
#include <glib.h>
#include "ringbuffer.h"

struct _RingBuffer
{
    GPtrArray *array;
    // gpointer *buffer;
    guint capacity;
    // guint isize;
    guint write_index;
    guint read_index;
};

RingBuffer *rbuf_sized_new(guint capacity)
{
    RingBuffer *rbuf = g_malloc0(sizeof(RingBuffer));
    rbuf->array = g_ptr_array_sized_new(capacity);
    // rbuf->buffer = g_malloc0_n(n_blocks, block_size);
    rbuf->capacity = capacity;
    // rbuf->isize = block_size;
    rbuf_reset(rbuf);
    return rbuf;
}

void rbuf_free(RingBuffer *rbuf)
{
    g_ptr_array_free(rbuf->array, TRUE);
    // g_free(rbuf->buffer);
    g_free(rbuf);
}

void rbuf_reset(RingBuffer *rbuf)
{
    rbuf->write_index = 0;
    rbuf->read_index = 0;
}

gboolean rbuf_read_available(RingBuffer *rbuf)
{
    return rbuf->read_index < rbuf->write_index;
}

static gpointer rbuf_get_read_pointer(RingBuffer *rbuf)
{
    return g_ptr_array_index(rbuf->array, rbuf->read_index % rbuf->capacity);
}

gpointer rbuf_read(RingBuffer *rbuf)
{
    if (rbuf_read_available(rbuf))
    {
        gpointer data = rbuf_get_read_pointer(rbuf);
        rbuf->read_index++;
        return data;
    }
    else
        return NULL;
}

gpointer rbuf_get_write_pointer(RingBuffer *rbuf)
{
    return g_ptr_array_index(rbuf->array, rbuf->write_index % rbuf->capacity);
}

void rbuf_advance(RingBuffer *rbuf)
{
    // if we are overwriting existing data under read_index, move it up to the next oldest item.
    if ((rbuf->write_index - rbuf->read_index) >= rbuf->capacity)
    {
        rbuf->read_index++;
    }
    rbuf->write_index++;
}

void rbuf_write(RingBuffer *rbuf, gpointer data)
{
    if (rbuf->array->len < rbuf->capacity)
        g_ptr_array_add(rbuf->array, data);
    else
    {
        gpointer olddata = rbuf->array->pdata[rbuf->write_index % rbuf->capacity];
        if (olddata != NULL)
        {
            g_free(olddata);
            olddata = NULL;
        }
        g_assert(olddata == NULL);
        rbuf->array->pdata[rbuf->write_index % rbuf->capacity] = data;
    }
    g_assert(rbuf->array->len <= rbuf->capacity);
    rbuf_advance(rbuf);
}

gpointer rbuf_write_replace(RingBuffer *rbuf, gpointer data)
{
    gpointer olddata = NULL;
    if (rbuf->array->len < rbuf->capacity)
        g_ptr_array_add(rbuf->array, data);
    else
    {
        olddata = rbuf->array->pdata[rbuf->write_index % rbuf->capacity];
        rbuf->array->pdata[rbuf->write_index % rbuf->capacity] = data;
    }
    rbuf_advance(rbuf);
    return olddata;
}

guint rbuf_length(RingBuffer *rbuf)
{
    return rbuf->write_index - rbuf->read_index;
}

gpointer rbuf_to_array(RingBuffer *rbuf)
{
    gpointer *array = malloc(sizeof(gpointer) * rbuf_length(rbuf));
    g_return_val_if_fail(array != NULL, NULL);

    for (guint index = 0; rbuf_read_available(rbuf); index++)
    {
        array[index] = rbuf_read(rbuf);
    }
    return array;
}

StackBuffer *rbuf_to_sbuf(RingBuffer *rbuf)
{
    StackBuffer *sbuf = sbuf_sized_new(rbuf_length(rbuf));
    g_return_val_if_fail(sbuf != NULL, NULL);

    while (rbuf_read_available(rbuf))
    {
        sbuf_push(sbuf, rbuf_read(rbuf));
    }
    return sbuf;
}