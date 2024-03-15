#ifndef QUEUE_H_
#define QUEUE_H_

#include "list.h"

typedef struct queue *queue;

// represents a queue data structure
struct queue {
    list head;  // front of the queue
    list tail;  // end of the queue
};

/* create an empty queue */
extern queue Queue(void);

/* insert an elem at the end of the queue */
extern void Enqueue(queue q, void *elem);

/* delete the front elem on the queue and return it */
extern void *Dequeue(queue q);

/* return a true value if and only if the queue is empty */
extern int EmptyQueue(queue q);

/* frees the entire queue */
extern void FreeQueue(queue q);

#endif /* QUEUE_H_ */
