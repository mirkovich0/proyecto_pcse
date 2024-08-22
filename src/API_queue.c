/*
 * API_queue.c
 *
 *  Created on: Aug 21, 2024
 *      Author: Mirko Serra
 */

#include "API_queue.h"

#include <stdlib.h>

queue_t* queue_New(size_t queue_size)
{
    queue_t* queue = (queue_t*) malloc(sizeof(queue_t) + queue_size - 1);
    queue->ini_ptr = 0;
    queue->end_ptr = 0;
    queue->queue_size = queue_size;
}

void queue_Put(queue_t* queue, uint8_t data)
{
    if (queue_Full(queue)) {
        return ;
    }
    queue->data[queue->end_ptr++] = data;
    if (queue->end_ptr == queue->queue_size) {
        queue->end_ptr = 0;
    }
}

uint8_t queue_Peek(queue_t* queue)
{
    return queue->data[queue->ini_ptr];
}

uint8_t queue_Get(queue_t* queue)
{
    uint8_t retval = queue_Peek(queue);
    queue_RemoveOne(queue);
    return retval;
}

void queue_RemoveOne(queue_t* queue)
{
    if (!queue_Empty(queue)) {
        queue->ini_ptr++;
        if (queue->ini_ptr == queue->queue_size) {
            queue->ini_ptr = 0;
        }
    }
}

bool queue_Full(const queue_t* queue)
{
    return (queue->end_ptr == (queue->queue_size-1) && 0 == queue->ini_ptr) || (queue->ini_ptr == (queue->end_ptr+1));
}

bool queue_Empty(const queue_t* queue)
{
    return queue->ini_ptr == queue->end_ptr;
}
