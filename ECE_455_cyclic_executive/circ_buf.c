#ifndef CIRC_BUF_H
#define CIRC_BUF_H
#include "circ_buf.h"

void buf_init(struct circ_buf* buf, uint32_t max_size)
{
    int i = 0;
    buf->buf = (void**) malloc (sizeof(void*) * max_size);
    for (i = 0; i < max_size; i++)
    {
        // init all pointers to 0
        buf->buf[i] = NULL;
    }
    buf->read_idx = 0;
    buf->write_idx = 0;
    buf->len = 0;
    buf->max_size = max_size;
}
void buf_write(struct circ_buf* buf, void* to_write)
{
    
    void** buffer = buf->buf;
    buffer[buf->write_idx] = to_write;

    // increment write index (with wrapping)
    buf->write_idx = (buf->write_idx == (buf->max_size - 1))? 0 : buf->write_idx + 1;
    
    if (buf->len == buf->max_size)
    {
        // discard oldest data, move read index
        if (buf->read_idx == (buf->max_size - 1))
        {
            // need to wrap read index
            buf->read_idx = 0;
        }
        else
        {
            buf->read_idx += 1;
        }
    }
    else
    {
        // added a new element
        buf->len += 1;
    }
    
    return;
}
void* buf_read(struct circ_buf* buf)
{
    void* to_return = NULL;
    if (buf->len == 0)
    {
        // buffer empty, do nothing
        to_return = NULL;
    }
    else
    {
        to_return = buf->buf[buf->read_idx];
        // clear element in buffer
        buf->buf[buf->read_idx] = NULL;
        buf->len -= 1;

        // update read index
        buf->read_idx = (buf->read_idx == (buf->max_size - 1))? 0 : buf->read_idx + 1;
    }

    return to_return;
}
void* buf_peek(struct circ_buf* buf)
{
    return buf->buf[buf->read_idx];
}
void print_circular_buffer(struct circ_buf* buf)
{
	uint32_t i = 0;
	uint32_t idx = 0;
  uint32_t num_elements = 0;  
	if (buf == NULL)
    {
        printf("Circular buffer is NULL.\n");
        return;
    }

    printf("Circular buffer contents:\n");

    // Calculate the number of elements in the buffer
    num_elements = (buf->len < buf->max_size) ? buf->len : buf->max_size;

    // Iterate through the buffer and print each element with its index
    for (i = 0; i < num_elements; i++)
    {
        // Calculate the index of the current element
        idx = (buf->read_idx + i) % buf->max_size;

        // Print the index and the element at that index
        printf("Index %u: %p\n", idx, buf->buf[idx]);
    }
}

#endif
