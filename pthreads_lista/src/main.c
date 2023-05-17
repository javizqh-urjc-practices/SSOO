#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <pthread.h>
#include "list.h"

enum {
	MAX_THREADS = 100
};

struct thread_args {
	int n_thread;
	struct List *list;
};

void *thread_action(void *arg);
int check_order(struct List *list);

int
main()
{
	// Declare variables
	int exit_value = EXIT_SUCCESS;
	int n_threads;
	pthread_t tid[MAX_THREADS];
	struct thread_args *targ_arr[MAX_THREADS];

	struct List *list = createEmptyList();

	for (n_threads = 0; n_threads < MAX_THREADS; n_threads++) {
		struct thread_args *args = malloc(sizeof(struct thread_args));

		if (args == NULL) {
			err(EXIT_FAILURE, "malloc failed");
		}
		memset(args, 0, sizeof(struct thread_args));

		args->list = list;
		args->n_thread = n_threads;
		targ_arr[n_threads] = args;
		pthread_create(&(tid[n_threads]), NULL, &thread_action,
			       targ_arr[n_threads]);
	}

	for (n_threads = 0; n_threads < MAX_THREADS; n_threads++) {
		pthread_join(tid[n_threads], NULL);
	}

	if (lengthList(list) == 60 * MAX_THREADS) {
		printf("Correct length: There are %d creats\n",
		       lengthList(list));
		if (check_order(list)) {
			printf("Correct order\n");
		} else {
			printf("Incorrect order\n");
			exit_value = EXIT_FAILURE;
		}
	} else {
		printf("Incorrect length %d\n", lengthList(list));
		exit_value = EXIT_FAILURE;
	}
	// Freeing memory
	freeList(list);
	free(list);

	// Exit
	exit(exit_value);
}

int
check_order(struct List *list)
{
	struct Creat *creat = extractfirst(list);
	int current_id[MAX_THREADS];

	memset(current_id, 0, sizeof(current_id));

	while (creat != NULL) {
		if (current_id[creat->id / MAX_THREADS] > creat->id) {
			return 0;	// Order is incorrect
		}
		current_id[creat->id / MAX_THREADS] = creat->id;
		free(creat);
		creat = extractfirst(list);
	}
	return 1;
}

void *
thread_action(void *arg)
{
	struct thread_args *args = (struct thread_args *)arg;
	int i;
	int other_extracted = 0;

	int min = 100 * args->n_thread;
	int max = 100 + min;

	for (i = min; i < max; i++) {
		insertLast(args->list, newCreat(i, "hi", 23, 3, "black"));
	}

	for (i = 0; i < 40; i++) {
		struct Creat *extracted = extractlast(args->list);

		if (extracted->id < min || extracted->id >= max) {
			other_extracted++;
		}
		free(extracted);
	}
	printf("Thread %d: other thread creats extracted: %d\n", args->n_thread,
	       other_extracted);
	free(args);
	return NULL;
}
