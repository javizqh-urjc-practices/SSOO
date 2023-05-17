#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

void exec_regex(char *regex, int pipe_fd[2]);
void exec_cmd(int argc, char **argv, int pipe_fd[2]);
char *find_path(char *command);
int command_exists(char *path);

void
usage()
{
	fprintf(stderr, "Usage: filterout <regex> <command> [options]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	argc--;
	argv++;
	if (argc < 2) {
		usage();
	}
	// Create pipe for child
	int pipe_fd[2] = { -1, -1 };
	if (pipe(pipe_fd)) {
		err(EXIT_FAILURE, "pipe failed");
	}
	// First fork
	pid_t regex_pid = fork();

	if (regex_pid < 0) {
		err(EXIT_FAILURE, "fork failed");
	}

	if (regex_pid == 0) {
		exec_regex(argv[0], pipe_fd);
	}

	argc--;
	argv++;

	// Second fork
	pid_t cmd_pid = fork();

	if (cmd_pid < 0) {
		err(EXIT_FAILURE, "fork failed");
	}

	if (cmd_pid == 0) {
		exec_cmd(argc, argv, pipe_fd);
	}
	// Close pipe
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	// Wait
	int i = 0;
	int wstatus;

	for (i = 0; i < 2; i++) {
		if (wait(&wstatus) == -1) {
			perror("waitpid failed");
			return EXIT_FAILURE;
		}
		if (WIFEXITED(wstatus)) {
			if (WEXITSTATUS(wstatus)) {
				return EXIT_FAILURE;
			}
		} else if (WIFSIGNALED(wstatus)) {
			printf("Killed by signal %d\n", WTERMSIG(wstatus));
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void
exec_regex(char *regex, int pipe_fd[2])
{
	close(pipe_fd[1]);

	// redirect stdin
	if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
		err(EXIT_FAILURE, "Failed to dup stdin");
	}
	close(pipe_fd[0]);

	execl("/usr/bin/egrep", "-E", regex, NULL);
	err(EXIT_FAILURE, "exec failed");
}

void
exec_cmd(int argc, char **argv, int pipe_fd[2])
{
	close(pipe_fd[0]);

	// redirect stdout
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
		err(EXIT_FAILURE, "Failed to dup stdout");
	}
	close(pipe_fd[1]);

	char *cmd = find_path(argv[0]);

	if (cmd == NULL) {
		fprintf(stderr, "%s not found\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *args[argc];

	args[0] = cmd;

	int i;

	for (i = 1; i < argc; i++) {
		args[i] = argv[i];
	}
	args[argc] = NULL;

	execv(cmd, args);
	free(cmd);
	err(EXIT_FAILURE, "exec failed");
}

char *
find_path(char *command)
{
	char *path = malloc(sizeof(char) * 256);

	if (path == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	memset(path, 0, sizeof(char) * 256);
	strcpy(path, "/bin/");

	strcat(path, command);

	if (command_exists(path)) {
		printf("foun in bin\n");
		return path;
	}

	memset(path, 0, sizeof(char) * 256);
	strcpy(path, "/usr/bin/");

	strcat(path, command);

	if (command_exists(path)) {
		printf("foun in usr bin\n");
		return path;
	}
	free(path);
	return NULL;
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
