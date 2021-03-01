#include <math.h>
#include <glib.h>
#include "ringbuffer.h"

struct _RingBuffer {
    gpointer *buffer;
    unsigned long capacity;
    unsigned long isize;
    unsigned long write_index;
    unsigned long read_index;
};

RingBuffer *rbuf_sized_new(gsize n_blocks, gsize block_size)
{
    RingBuffer *rbuf = g_malloc0(sizeof(RingBuffer));
    rbuf->buffer = g_malloc0_n(n_blocks, block_size);
    rbuf->capacity = n_blocks;
    rbuf->isize = block_size;
    rbuf_reset(rbuf);
    return rbuf;
}

void rbuf_free(RingBuffer *rbuf)
{
    // g_ptr_array_free(rbuf->array, TRUE);
    g_free(rbuf->buffer);
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

static gsize __index(RingBuffer *rbuf, gsize index)
{
    return rbuf->isize * index;
}

static gsize __read_index(RingBuffer *rbuf)
{
    return __index(rbuf, rbuf->read_index);
}

static gsize __write_index(RingBuffer *rbuf)
{
    return __index(rbuf, rbuf->write_index);
}

static gsize __read_index_mod(RingBuffer *rbuf, gsize mod)
{
    return __index(rbuf, rbuf->read_index % mod);
}

static gsize __write_index_mod(RingBuffer *rbuf, gsize mod)
{
    return __index(rbuf, rbuf->write_index % mod);
}

static gsize __read_index_capped(RingBuffer *rbuf)
{
    return __read_index_mod(rbuf, rbuf->capacity);
}

static gsize __write_index_capped(RingBuffer *rbuf)
{
    return __write_index_mod(rbuf, rbuf->capacity);
}

static gpointer rbuf_get_read_pointer(RingBuffer *rbuf)
{
    return rbuf->buffer[__read_index_capped(rbuf)];
}

gpointer rbuf_read(RingBuffer *rbuf)
{
    if (rbuf_read_available(rbuf))
    {
        gpointer data = rbuf_get_read_pointer(rbuf);
        rbuf->read_index++;
        return data;
    }
    else return NULL;
}

static gpointer rbuf_get_write_pointer(RingBuffer *rbuf)
{
    return rbuf->buffer[__write_index_capped(rbuf)];
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
    // if (rbuf->array->len < rbuf->capacity)
    //     g_ptr_array_add(rbuf->array, data);
    // else
    //     rbuf->array->pdata[rbuf->write_index % rbuf->capacity] = data;
    rbuf->buffer[__write_index_capped(rbuf)] = data;
    rbuf_advance(rbuf);
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