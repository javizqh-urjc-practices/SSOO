#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

enum {
	MAX_ARG_SIZE = 1024,
	MAX_ARGS = 64,
	N_COMMANDS = 3
};

struct command {
	char argv[MAX_ARGS][MAX_ARG_SIZE];
	int argc;
	pid_t pid;
	int pipe_in[2];
	int pipe_out[2];
	struct command *pipe_next;
};

struct command *new_command(char *command);
int find_path(char *command);
int command_exists(char *command);
int pipe_commands(struct command *in_command, struct command *out_command);
int close_pipe(int fd);
int close_unused_pipes(struct command *current_command,
		       struct command *first_command);
int close_all_pipes(struct command *first_command);
void redirect_stdin(struct command *command);
void redirect_stdout(struct command *command);
void exec_child(struct command *first_command, struct command *command);
int wait_for_child(struct command *last_command);

void
usage()
{
	fprintf(stderr, "usage: pipelines <COMMAND> <COMMAND> <COMMAND>\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	int i;
	int exit_status = 0;

	argc--;
	argv++;
	if (argc != N_COMMANDS) {
		usage();
	}
	// Create commands
	struct command *first_cmd = new_command(argv[0]);
	struct command *second_cmd = new_command(argv[1]);
	struct command *third_cmd = new_command(argv[2]);

	// Pipe them
	pipe_commands(first_cmd, second_cmd);
	pipe_commands(second_cmd, third_cmd);

	// Fork
	struct command *current_cmd = first_cmd;

	for (i = 0; i < N_COMMANDS; i++) {
		current_cmd->pid = fork();
		if (current_cmd->pid != 0 && current_cmd->pipe_next != NULL) {
			current_cmd = current_cmd->pipe_next;
		} else {
			break;
		}
	}

	switch (current_cmd->pid) {
	case -1:
		//ERROR
		err(EXIT_FAILURE, "fork failed");
		break;
	case 0:
		// Child
		exec_child(first_cmd, current_cmd);
		break;
	default:
		// Parent
		close_all_pipes(first_cmd);
		exit_status = wait_for_child(third_cmd);
		break;
	}

	// Free commands
	free(first_cmd);
	free(second_cmd);
	free(third_cmd);
	return exit_status;
}

struct command *
new_command(char *command)
{
	int i = 0;
	char *token;
	struct command *new_command =
	    (struct command *)malloc(sizeof(struct command));

	if (new_command == NULL) {
		err(EXIT_FAILURE, "maloc failed");
	}

	memset(new_command, 0, sizeof(struct command));

	while ((token = strtok_r(command, " ", &command))) {
		strcpy(new_command->argv[i], token);
		i++;
	}
	new_command->argc = i;
	new_command->pid = 0;
	new_command->pipe_in[0] = -1;
	new_command->pipe_in[1] = -1;
	new_command->pipe_out[0] = -1;
	new_command->pipe_out[1] = -1;
	new_command->pipe_next = NULL;

	return new_command;
}

int
find_path(char *command)
{
	int index;
	char *paths_to_search[2] = { "/bin/", "/usr/bin/" };
	char *path = malloc(MAX_ARG_SIZE);

	if (path == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}

	for (index = 0; index < 2; index++) {
		memset(path, 0, MAX_ARG_SIZE);
		strcpy(path, paths_to_search[index]);

		strcat(path, command);

		if (command_exists(path)) {
			strcpy(command, path);
			free(path);
			return 1;
		}
	}

	free(path);
	return 0;
};

int
command_exists(char *path)
{
	struct stat buf;

	if (stat(path, &buf) != 0) {
		return 0;
	}
	// Check the `st_mode` field to see if the `S_IXUSR` bit is set
	if (buf.st_mode & S_IXUSR) {
		return 1;
	}
	return 0;
}

int
pipe_commands(struct command *in_command, struct command *out_command)
{
	int fd[2];

	if (pipe(fd) < 0) {
		err(EXIT_FAILURE, "Failed to pipe");
	}
	in_command->pipe_next = out_command;
	in_command->pipe_out[0] = fd[0];
	in_command->pipe_out[1] = fd[1];
	out_command->pipe_in[0] = fd[0];
	out_command->pipe_in[1] = fd[1];
	return 1;
}

int
close_pipe(int fd)
{
	if (fd >= 0) {
		close(fd);
	}
	return 1;
}

int
close_unused_pipes(struct command *current_command,
		   struct command *first_command)
{
	struct command *pipe_command = first_command;

	while (pipe_command != NULL) {
		if (pipe_command == current_command) {
			close_pipe(pipe_command->pipe_in[1]);
			close_pipe(pipe_command->pipe_out[0]);
		} else {
			// Close if the cmd output is not the next input
			if (pipe_command->pipe_in[1] !=
			    current_command->pipe_out[1]) {
				close_pipe(pipe_command->pipe_in[1]);
			}
			// Close if the cmd input is not the prev output
			if (pipe_command->pipe_out[0] !=
			    current_command->pipe_in[0]) {
				close_pipe(pipe_command->pipe_out[0]);
			}
			close_pipe(pipe_command->pipe_in[0]);
			close_pipe(pipe_command->pipe_out[1]);
		}
		pipe_command = pipe_command->pipe_next;
	}
	return 1;
}

int
close_all_pipes(struct command *first_command)
{
	struct command *current_command = first_command;

	while (current_command != NULL) {
		close_pipe(current_command->pipe_in[0]);
		close_pipe(current_command->pipe_in[1]);
		close_pipe(current_command->pipe_out[0]);
		close_pipe(current_command->pipe_out[1]);
		current_command = current_command->pipe_next;
	}
	return 1;
}

// Redirect input and output: Child
void
redirect_stdin(struct command *command)
{
	if (command->pipe_in[0] == -1)
		return;
	if (dup2(command->pipe_in[0], STDIN_FILENO) == -1) {
		err(EXIT_FAILURE, "Failed to dup stdin");
	}
	close_pipe(command->pipe_in[0]);
}

void
redirect_stdout(struct command *command)
{
	if (command->pipe_out[1] == -1)
		return;
	if (dup2(command->pipe_out[1], STDOUT_FILENO) == -1) {
		err(EXIT_FAILURE, "Failed to dup stdout");
	}
	close_pipe(command->pipe_out[1]);
}

void
exec_child(struct command *first_command, struct command *command)
{
	int i = 0;
	char *args[MAX_ARG_SIZE];

	close_unused_pipes(command, first_command);

	if (find_path(command->argv[0])) {
		redirect_stdin(command);
		redirect_stdout(command);
		for (i = 0; i < command->argc; i++) {
			args[i] = command->argv[i];
		}
		args[command->argc] = NULL;
		execv(args[0], args);
		err(EXIT_FAILURE, "execv failed");
	}
	free(first_command->pipe_next->pipe_next);
	free(first_command->pipe_next);
	free(first_command);
	exit(EXIT_FAILURE);
}

int
wait_for_child(struct command *last_command)
{
	int i;
	int wstatus;
	pid_t wait_pid;

	for (i = 0; i < N_COMMANDS; i++) {
		wait_pid = wait(&wstatus);
		if (wait_pid == -1) {
			perror("waitpid failed");
			return EXIT_FAILURE;
		}
		if (WIFEXITED(wstatus)) {
			if (wait_pid == last_command->pid) {
				return WEXITSTATUS(wstatus);
			}
		}
	}
	return EXIT_SUCCESS;
}
