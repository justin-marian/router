#include "../utils/queue.h"

#include <stdlib.h>

queue Queue(void) {
	queue q = malloc(sizeof(struct queue));
	q->head = q->tail = NULL;
	return q;
}

int EmptyQueue(queue q) {
	return !q->head;
}

void Enqueue(queue q, void *elem) {
	if(EmptyQueue(q)) {
		q->head = q->tail = InsertElem(elem, NULL);
		return;
	}

	q->tail->next = InsertElem(elem, NULL);
	q->tail = q->tail->next;
}

void *Dequeue(queue q) {
	if(EmptyQueue(q)) return NULL;

	void *temp = q->head->elem;
	q->head = DeleteElem(q->head);

	return temp;
}

void FreeQueue(queue q) {
	while (!EmptyQueue(q))
		(void)Dequeue(q);
	free(q);
}
