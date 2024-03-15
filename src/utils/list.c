#include "../utils/list.h"

#include <stdlib.h>

list InsertElem(void *elem, list l) {
	list temp = malloc(sizeof(struct cell));
	temp->elem = elem;
	temp->next = l;
	return temp;
}

list DeleteElem(list l) {
	list temp = l->next; 
	free(l);
	return temp;
}
