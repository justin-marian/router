#ifndef LIST_H_
#define LIST_H_

typedef struct cell *list;

// represents a cell in a singly linked list
struct cell {
    void  *elem; // data element
    list  next;  // next cell in the list
};

/* create a new list cell with the given element and appends it to the list */
extern list InsertElem(void *elem, list l);

/* frees the first element of the list */
extern list DeleteElem(list l);

#endif /* LIST_H_ */
