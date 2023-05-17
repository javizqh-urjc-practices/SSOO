#include "listcreat.h"

int
main(int argc, char *argv[])
{
	// Declare variables
	int index;
	struct Creat c1 = newCreat(1, "hi", 23, 3, "black");
	struct Creat c2 = newCreat(2, "fgi", 53, 5, "blk");
	struct Creat c3 = newCreat(3, "3i", 26, 34, "white");
	struct Creat c4 = newCreat(4, "3i", 26, 34, "white");
	struct Creat c5 = newCreat(5, "hi", 23, 3, "black");
	struct Creat c6 = newCreat(6, "fgi", 53, 5, "blk");
	struct Creat c7 = newCreat(7, "3i", 26, 34, "white");
	struct Creat c8 = newCreat(8, "3i", 26, 34, "white");
	struct Creat c9 = newCreat(9, "3i", 26, 34, "white");
	struct Creat c10 = newCreat(10, "3i", 26, 34, "white");

	// Declared array
	const struct Creat animals[10] =
	    { c1, c2, c3, c4, c5, c6, c7, c8, c9, c10 };

	// Initialize the list and queue
	struct List list = createEmptyList();
	struct Queue queue = createEmptyQueue();

	// Doing things with the list

	for (index = 0; index < 10; index++) {
		insertFirst(&list, &animals[index]);
		enqueue(&queue, &animals[index]);
	}
	printList(&list);

	int listLength = lengthList(&list);

	for (index = 0; index < listLength; index++) {
		if (index % 2 == 0) {
			delete(&list, animals[index].id);
		}
	}
	printList(&list);

	// Doing things with the queue

	printQueue(&queue);

	int queueLength = lengthQueue(&queue);

	struct Creat *toFree;

	for (index = 0; index < queueLength; index++) {
		if (index >= 5) {
			break;
		}
		toFree = dequeue(&queue);
		printCreat(toFree);
		printf("\n");
		free(toFree);
	}

	// Freeing memory
	freeList(&list);
	freeQueue(&queue);

	// Exit
	exit(EXIT_SUCCESS);
}
