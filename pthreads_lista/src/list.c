#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <pthread.h>
#include "list.h"

// Creat
struct Creat *
newCreat(long long id, char *specie, int height, int weight, char *color)
{
	struct Creat *creat = malloc(sizeof(struct Creat));

	if (creat == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	memset(creat, 0, sizeof(struct Creat));

	creat->id = id;
	strcpy(creat->specie, specie);
	creat->height = height;
	creat->weight = weight;
	strcpy(creat->color, color);
	creat->next = NULL;
	creat->prev = NULL;

	return creat;
}

void
printCreat(struct Creat *creat)
{
	printf("Id: %lld,", creat->id);
	printf("Specie: %s,", creat->specie);
	printf("Height: %d,", creat->height);
	printf("Weight: %d,", creat->weight);
	printf("Color: %s", creat->color);
}

// -------------------------------------

// List
struct List *
createEmptyList()
{
	struct List *list = malloc(sizeof(struct List));

	if (list == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	memset(list, 0, sizeof(struct List));

	list->head = NULL;
	list->last = NULL;
	if (pthread_mutex_init(&list->lock, NULL) != 0) {
		err(EXIT_FAILURE, "mutex lock failed");
	}
	return list;
}

int
freeList(struct List *list)
{
	pthread_mutex_lock(&list->lock);
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
	pthread_mutex_unlock(&list->lock);
	pthread_mutex_destroy(&list->lock);
	return 1;
}

int
isEmptyList(struct List *list)
{
	return list->head == NULL;
}

static void
printList_t(struct List *list)
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

void
printList(struct List *list)
{
	pthread_mutex_lock(&list->lock);
	printList_t(list);
	pthread_mutex_unlock(&list->lock);
}

static int
insertFirst_t(struct List *list, struct Creat *data)
{
	//if the list is empty then set head and last to data
	if (isEmptyList(list)) {
		//point it to old first node
		data->next = list->head;

		//point first to new first node
		list->head = data;
		list->last = data;
	} else {
		// if the list has data
		//point it to old first node
		data->next = list->head;

		//point head prev to data
		list->head->prev = data;

		//point first to new first node
		list->head = data;
	}
	return 1;
}

int
insertFirst(struct List *list, struct Creat *data)
{
	int return_val;

	pthread_mutex_lock(&list->lock);
	return_val = insertFirst_t(list, data);
	pthread_mutex_unlock(&list->lock);
	return return_val;
}

static int
insertLast_t(struct List *list, struct Creat *data)
{
	//if the list is empty then set head and last to data
	if (isEmptyList(list)) {
		//point it to old first node
		data->prev = list->last;

		//point first to new first node
		list->head = data;
		list->last = data;
	} else {
		// if the list has data

		// Check if the data is already in a list
		if (data->next != NULL || data->prev != NULL) {
			return 0;
		}
		//point it to old first node
		data->prev = list->last;

		//point head prev to data
		list->last->next = data;

		//point first to new first node
		list->last = data;
	}
	return 1;
}

int
insertLast(struct List *list, struct Creat *data)
{
	int return_val;

	pthread_mutex_lock(&list->lock);
	return_val = insertLast_t(list, data);
	pthread_mutex_unlock(&list->lock);
	return return_val;
}

struct Creat *
getFirst(struct List *list)
{
	return list->head;
}

struct Creat *
getLast(struct List *list)
{
	return list->last;
}

struct Creat *
extractfirst(struct List *list)
{
	pthread_mutex_lock(&list->lock);

	// If list is empty, exit 0
	if (list->head == NULL) {
		pthread_mutex_unlock(&list->lock);
		return NULL;
	}

	struct Creat *first = list->head;

	list->head = list->head->next;
	if (list->head != NULL) {
		list->head->prev = NULL;
	}

	pthread_mutex_unlock(&list->lock);
	return first;
}

struct Creat *
extractlast(struct List *list)
{
	pthread_mutex_lock(&list->lock);

	// If list is empty, exit 0
	if (list->head == NULL) {
		pthread_mutex_unlock(&list->lock);
		return NULL;
	}

	struct Creat *last = list->last;

	list->last = list->last->prev;
	list->last->next = NULL;

	pthread_mutex_unlock(&list->lock);
	return last;
}

static int
lengthList_t(struct List *list)
{
	int lengthList = 0;
	struct Creat *current;

	for (current = list->head; current != NULL; current = current->next) {
		lengthList++;
	}

	return lengthList;
}

int
lengthList(struct List *list)
{
	int return_val;

	pthread_mutex_lock(&list->lock);
	return_val = lengthList_t(list);
	pthread_mutex_unlock(&list->lock);
	return return_val;
}

static struct Creat *
find_t(struct List *list, long long key)
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

struct Creat *
find(struct List *list, long long key)
{
	struct Creat *return_val;

	pthread_mutex_lock(&list->lock);
	return_val = find_t(list, key);
	pthread_mutex_unlock(&list->lock);
	return return_val;
}

static int
delete_t(struct List *list, long long key)
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

int
delete(struct List *list, long long key)
{
	int return_val;

	pthread_mutex_lock(&list->lock);
	return_val = delete_t(list, key);
	pthread_mutex_unlock(&list->lock);
	return return_val;
}
