/*
 * API_queue.h
 *
 *  Created on: Aug 21, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_QUEUE_H_
#define API_INC_API_QUEUE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t ini_ptr;
    size_t end_ptr;
    size_t queue_size;
    uint8_t data[1];
} queue_t;

queue_t* queue_New(size_t queue_size);
void queue_Put(queue_t* queue, uint8_t data);
uint8_t queue_Peek(queue_t* queue);
uint8_t queue_Get(queue_t* queue);
void queue_RemoveOne(queue_t* queue);
bool queue_Full(const queue_t* queue);
bool queue_Empty(const queue_t* queue);

#endif /* API_INC_API_QUEUE_H_ */
