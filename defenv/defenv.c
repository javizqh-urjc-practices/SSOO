#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

int get_command_index(int argc, char **argv);
void set_env_var(int argc, char **argv);

int
main(int argc, char *argv[])
{
	pid_t child_pid;
	int wstatus;

	// ------- Parse command line arguments -------------
	// Get command to execute index
	int command_to_exec_index = get_command_index(argc, argv);

	// Check if in arguments there is a program to execute
	if (command_to_exec_index == argc) {
		fprintf(stderr, "defenv: Not passed program to execute\n");
		exit(EXIT_FAILURE);
	}
	// Set command to execute
	const char *command_to_exec = argv[command_to_exec_index];

	// Set command args
	int args_index;
	char *args[argc - command_to_exec_index + 1];

	for (args_index = 0; args_index < argc - command_to_exec_index;
	     args_index++) {
		args[args_index] = argv[args_index + command_to_exec_index];
	}

	args[argc - command_to_exec_index] = NULL;
	// --------------------------------------------------

	// Fork the process
	child_pid = fork();

	// Check for error
	if (child_pid == -1) {
		// Fork failed
		err(child_pid, "Failed to fork");
		exit(EXIT_FAILURE);
	}
	// Wait for the child process
	if (child_pid == 0) {
		// We are in the child process
		// Set environment variables
		set_env_var(argc, argv);
		execv(command_to_exec, args);
		// If exec has return print error
		err(child_pid, "Failed to exec");
		exit(EXIT_FAILURE);
	} else {
		// We are in the parent process
		pid_t wtr = waitpid(child_pid, &wstatus, WUNTRACED);

		if (wtr == -1) {
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(wstatus)) {
			if (WEXITSTATUS(wstatus)) {
				// Exit with error
				printf("defenv: program failed %s\n",
				       command_to_exec);
			}
		} else if (WIFSIGNALED(wstatus)) {
			printf("Killed by signal %d\n", WTERMSIG(wstatus));
		}
	}

	// Return with the same status as the child
	return WEXITSTATUS(wstatus);
}

int
get_command_index(int argc, char **argv)
{
	// Parse command line arguments
	int i;
	char *matched_ptr = NULL;

	for (i = 1; i < argc; i++) {
		matched_ptr = strchr(argv[i], '=');
		// If NULL then not matched
		if (matched_ptr == NULL) {
			// Return the position of the command
			return i;
		}
	}
	// If no environment passed
	return argc;
}

void
set_env_var(int argc, char **argv)
{
	// Parse command line arguments
	int i;
	char *matched_ptr = NULL;

	for (i = 1; i < argc; i++) {
		matched_ptr = strchr(argv[i], '=');
		// If NULL then not matched
		if (matched_ptr == NULL) {
			return;
		}

		char name[strlen(argv[i]) - strlen(matched_ptr)];

		strncpy(name, argv[i], strlen(argv[i]) - strlen(matched_ptr));
		name[strlen(argv[i]) - strlen(matched_ptr)] = '\0';

		char *value = matched_ptr + 1;

		value[strlen(matched_ptr) - 1] = '\0';

		// Set environment variables
		setenv(name, value, 1);
	}
}
