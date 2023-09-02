// simple circular buffer implementation for the purpose of scheduling
#include <stdint.h>
#include "stdlib.h"
#include "stdio.h"

typedef struct circ_buf
{
    void** buf;
    uint32_t read_idx;
    uint32_t write_idx;
    uint32_t len;
    uint32_t max_size;
} my_circ_buf;

// initializes values of an existing buffer
void buf_init(struct circ_buf* buf, uint32_t max_size);
// writes value to buffer, wraps around if full
void buf_write(struct circ_buf* buffer_struct, void* to_write);
// reads buffer at read index and removes it from the buffer
void* buf_read(struct circ_buf* buf);
// returns a pointer to the item at the read index but doesn't remove it from the queue
void* buf_peek(struct circ_buf* buf);
void print_circular_buffer(struct circ_buf* buf);
