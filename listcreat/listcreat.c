#include "listcreat.h"

// Creat
struct Creat
newCreat(long long id, char specie[MAX_SIZE_STRING], int height, int weight,
	 char color[MAX_SIZE_STRING])
{
	struct Creat creat;

	creat.id = id;
	strcpy(creat.specie, specie);
	creat.height = height;
	creat.weight = weight;
	strcpy(creat.color, color);
	creat.next = NULL;
	creat.prev = NULL;

	return creat;
}

void
printCreat(const struct Creat *creat)
{
	printf("Id: %lld,", creat->id);
	printf("Specie: %s,", creat->specie);
	printf("Height: %d,", creat->height);
	printf("Weight: %d,", creat->weight);
	printf("Color: %s", creat->color);
}

// -------------------------------------

// List
struct List
createEmptyList()
{
	struct List list = { NULL, NULL };
	return list;
}

int
freeList(struct List *list)
{
	//start from the first link
	struct Creat *current = list->head;
	struct Creat *previous = NULL;

	//if list is empty
	if (list->head == NULL) {
		return 0;
	}
	//navigate through list
	while (current != NULL) {
		// Store reference to current link
		previous = current;
		//go to next link
		current = current->next;
		// Delete previous
		free(previous);
	}

	return 1;
}

int
isEmptyList(const struct List *list)
{
	return list->head == NULL;
}

void
printList(const struct List *list)
{
	struct Creat *current = list->head;

	printf("\n[ ");

	//start from the beginning
	for (current = list->head; current != NULL; current = current->next) {
		printf("(");
		printCreat(current);
		printf(")");
	}

	printf(" ]\n");
}

int
insertFirst(struct List *list, const struct Creat *data)
{
	struct Creat *add = (struct Creat *)malloc(sizeof(struct Creat));

	// Check if malloc failed
	if (add == NULL) {
		return -1;
	}

	*add = *data;

	//if the list is empty then set head and last to data
	if (isEmptyList(list)) {
		//point it to old first node
		add->next = list->head;

		//point first to new first node
		list->head = add;
		list->last = add;
	} else {
		// if the list has data
		//point it to old first node
		add->next = list->head;

		//point head prev to data
		list->head->prev = add;

		//point first to new first node
		list->head = add;
	}

	return 1;
}

int
insertLast(struct List *list, const struct Creat *data)
{
	struct Creat *add = (struct Creat *)malloc(sizeof(struct Creat));

	// Check if malloc failed
	if (add == NULL) {
		return -1;
	}

	*add = *data;

	//if the list is empty then set head and last to data
	if (isEmptyList(list)) {
		//point it to old first node
		add->prev = list->last;

		//point first to new first node
		list->head = add;
		list->last = add;
	} else {
		// if the list has data

		// Check if the data is already in a list
		if (add->next != NULL || add->prev != NULL) {
			return 0;
		}
		//point it to old first node
		add->prev = list->last;

		//point head prev to data
		list->last->next = add;

		//point first to new first node
		list->last = add;
	}

	return 1;
}

struct Creat *
getFirst(const struct List *list)
{
	return list->head;
}

struct Creat *
getLast(const struct List *list)
{
	return list->last;
}

int
lengthList(const struct List *list)
{
	int lengthList = 0;
	struct Creat *current;

	for (current = list->head; current != NULL; current = current->next) {
		lengthList++;
	}

	return lengthList;
}

struct Creat *
find(const struct List *list, long long key)
{

	//start from the first link
	struct Creat *current = list->head;
	struct Creat *found = NULL;

	//if list is empty
	if (list->head == NULL) {
		return NULL;
	}
	// Number of matches of the key
	int nMatches = 0;

	// Navigate through list
	for (current = list->head; current != NULL; current = current->next) {
		if (current->id == key) {
			found = current;
			nMatches++;
		}
	}

	// Found more than one match
	if (nMatches > 1) {
		fprintf(stderr, "Error: %d matches of key %lld\n", nMatches,
			key);
		return NULL;
	}
	//if data found, return the current Link
	return found;
}

int
delete(struct List *list, long long key)
{
	struct Creat *current = list->head;
	struct Creat *previous = NULL;
	struct Creat *next_node = NULL;

	// If list is empty, exit 0
	if (list->head == NULL) {
		return 0;
	}
	// Navigate through list until id matches key
	while (current->id != key) {

		// If it reachs the end of the list, exit 0
		if (current->next == NULL) {
			return 0;
		} else {
			// Store reference to current link
			previous = current;
			// Move to next link
			current = current->next;
		}
	}

	// Found a match, update the link
	if (current == list->head) {
		// If the element to delete is the head
		// Change first to point to next link
		list->head = list->head->next;
		// Point previous to null
		list->head->prev = NULL;
	} else if (current->next == NULL) {
		// If the element to delete is the last
		// Change last to point to next link
		list->last = list->last->prev;
		// Point previous to null
		list->last->next = NULL;
	} else {
		// Bypass the current link
		previous->next = current->next;
		// Set next previous to previous link
		next_node = current->next;
		next_node->prev = previous;
	}

	// Free current
	free(current);

	return 1;
}

// -------------------------------------

// Queue
struct Queue
createEmptyQueue()
{
	struct List *listQueue = (struct List *)malloc(sizeof(struct List));

	// Check if malloc failed
	if (listQueue == NULL) {
		exit(EXIT_FAILURE);
	}

	*listQueue = createEmptyList();
	struct Queue queue = { listQueue };

	return queue;
}

int
freeQueue(struct Queue *queue)
{
	if (freeList(queue->list)) {
		free(queue->list);
		return 1;
	}
	return 0;
}

int
isEmptyQueue(struct Queue *queue)
{
	return isEmptyList(queue->list);
}

void
printQueue(struct Queue *queue)
{
	printList(queue->list);
}

int
lengthQueue(struct Queue *queue)
{
	return lengthList(queue->list);
}

int
enqueue(struct Queue *queue, const struct Creat *data)
{
	return insertFirst(queue->list, data);
}

struct Creat *
dequeue(struct Queue *queue)
{
	// If queue is empty, exit 0
	if (queue->list->head == NULL) {
		return 0;
	}
	// Get data to remove
	struct Creat *current = getLast(queue->list);

	// Update the link
	// Change last to point to next link
	queue->list->last = queue->list->last->prev;
	// Point previous to null
	queue->list->last->next = NULL;

	return current;
}

// -------------------------------------
