#include<stdlib.h>
#include <stdio.h>
#include <string.h>

// Define the max size of the string
enum {
	MAX_SIZE_STRING = 256
};

// ------------- Creat -----------------
struct Creat {
	long long id;		// Acts as key
	char specie[MAX_SIZE_STRING];
	int height;
	int weight;
	char color[MAX_SIZE_STRING];

	struct Creat *next;
	struct Creat *prev;
};

/**
 * @brief Creates a new instance of Creat
 * 
 * @param id long long 
 * @param specie string of max size 256
 * @param height int
 * @param weight int
 * @param color string of max size 256
 * @return struct Creat 
 */
struct Creat newCreat(long long id, char specie[MAX_SIZE_STRING], int height,
		      int weight, char color[MAX_SIZE_STRING]);

/**
 * @brief Prints the creat struct
 * 
 * @param creat Pointer to creat 
 */
void printCreat(const struct Creat *creat);

// -------------------------------------

// -------------- List -----------------
struct List {
	struct Creat *head;
	struct Creat *last;
};

/**
 * @brief Create an empty List struct
 * 
 * @return struct List 
 */
struct List createEmptyList();

/**
 * @brief Frees the memory of the list
 * 
 * @param list List to free
 * @return 0 = empty / 1 = Succesful
 */
int freeList(struct List *list);

/**
 * @brief Returns if the list is empty
 * 
 * @param list 
 * @return 0 = false / 1 = true
 */
int isEmptyList(const struct List *list);

/**
 * @brief Prints the contents of the list
 * 
 * @param list 
 */
void printList(const struct List *list);

/**
 * @brief Inserts Creat at the beginnning of the list
 * 
 * @param list 
 * @param data Of type Creat *
 * @return -1 = Malloc failed / 1 = Success
 */
int insertFirst(struct List *list, const struct Creat *data);

/**
 * @brief Inserts Creat at the end of the list
 * 
 * @param list 
 * @param data Of type Creat *
 * @return -1 = Malloc failed / 1 = Success
 */
int insertLast(struct List *list, const struct Creat *data);

/**
 * @brief Get the First Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *getFirst(const struct List *list);

/**
 * @brief Get the Last Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *getLast(const struct List *list);

/**
 * @brief Gets the length of the list
 * 
 * @param list 
 * @return int: Length
 */
int lengthList(const struct List *list);

/**
 * @brief Finds the Creat in the list
 * 
 * @param list 
 * @param key Also know as Creat id
 * @return struct Creat* = Succes / NULL = Failed
 *
 * @note If found more than 1 match return NULL
 */
struct Creat *find(const struct List *list, long long key);

/**
 * @brief Deletes the Creat in list that matches the key
 * 
 * @param list 
 * @param key Also know as Creat id
 * @return 0 = Failed / 1 = Success 
 */
int delete(struct List *list, long long key);

// -------------------------------------

// ------------- Queue -----------------
struct Queue {
	struct List *list;
};

/**
 * @brief Create an empty Queue struct
 * 
 * @return struct Queue
 *
 * @exception If malloc fails exit with EXIT_FAILURE
 */
struct Queue createEmptyQueue();

/**
 * @brief Frees the memory of the queue
 * 
 * @param queue Queue to free
 * @return 0 = empty / 1 = Succesful
 */
int freeQueue(struct Queue *queue);

/**
 * @brief Returns if the queue is empty
 * 
 * @param queue 
 * @return 0 = false / 1 = true
 */
int isEmptyQueue(struct Queue *queue);

/**
 * @brief Prints the contents of the queue
 * 
 * @param queue 
 */
void printQueue(struct Queue *queue);

/**
 * @brief Gets the length of the queue
 * 
 * @param queue 
 * @return int: Length
 */
int lengthQueue(struct Queue *queue);

/**
 * @brief Add Creat to the queue
 * 
 * @param queue 
 * @param data Of type Creat *
 * @return -1 = Malloc failed / 1 = Success 
 */
int enqueue(struct Queue *queue, const struct Creat *data);

/**
 * @brief Extracts the last Creat from the queue
 * 
 * @param queue 
 * @return struct Creat * 
 *
 * @note The user needs to free the memory of the return
 */
struct Creat *dequeue(struct Queue *queue);

// -------------------------------------
