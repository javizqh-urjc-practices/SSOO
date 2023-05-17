#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

struct ping_command {
	pid_t ping_pid;
	char *machine;
};

int is_num(char *str);
int wait_for_childs(int n_pings, struct ping_command child_ping[]);

int
main(int argc, char *argv[])
{
	// ------- Parse command line arguments -------------
	argv++;
	argc--;
	// Check if correct of machines to ping
	if (argc < 2) {
		printf("pinger: ERROR: not enough machines to ping\n");
		return EXIT_FAILURE;
	}
	// First see if the arguments are correct
	if (!is_num(argv[0])) {
		// If the first argument is a machine then error
		printf("Usage: pinger <Seconds to wait> [Machines...]\n");
		return EXIT_FAILURE;
	}
	// Set number of pings
	int n_pings = argc - 1;
	struct ping_command child_ping[n_pings];

	// --------------------------------------------------

	// To fork the process, only in the parent one
	int n_forks_done = 0;

	do {
		n_forks_done++;
		child_ping[n_forks_done - 1].ping_pid = fork();
		child_ping[n_forks_done - 1].machine = argv[n_forks_done];
	} while (child_ping[n_forks_done - 1].ping_pid != 0
		 && n_forks_done < n_pings);

	// Check for errors during fork
	if (child_ping[n_forks_done - 1].ping_pid == -1) {
		// Fork failed
		err(child_ping[n_forks_done - 1].ping_pid, "Failed to fork");
		exit(EXIT_FAILURE);
	}
	// Wait for the child process
	if (child_ping[n_forks_done - 1].ping_pid == 0) {
		// We are in the child process
		// Set ping arguments
		char *args[8];

		args[0] = "/bin/ping";
		args[1] = "-c";	// Number of ECHO_REQUEST
		args[2] = "2";	// Number of ECHO_REQUEST
		args[3] = "-q";
		args[4] = "-W";	// Seconds to wait
		args[5] = argv[0];	// Seconds to wait
		args[6] = argv[n_forks_done];
		args[7] = NULL;

		execv("/bin/ping", args);
		// If exec has return print error
		err(child_ping[n_forks_done - 1].ping_pid, "Failed to exec");
		exit(EXIT_FAILURE);
	} else {
		// We are in the parent process
		return wait_for_childs(n_pings, child_ping);
	}

	return 0;
}

int
is_num(char *str)
{
	int str_len = strlen(str);
	int i;

	for (i = 0; i < str_len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return 0;
		}
	}
	return 1;
}

int
wait_for_childs(int n_pings, struct ping_command child_ping[])
{
	pid_t wtr;
	int wstatus;
	int proc_finished;

	for (proc_finished = 0; proc_finished < n_pings; proc_finished++) {
		wtr = wait(&wstatus);
		if (wtr == -1) {
			perror("waitpid failed");
			return EXIT_FAILURE;
		}
		if (WIFEXITED(wstatus)) {
			if (WEXITSTATUS(wstatus)) {
				// Exit with error
				// Kill the other child process
				int child_index;

				// Find the problem child to print out
				for (child_index = 0; child_index < n_pings;
				     child_index++) {
					if (child_ping[child_index].ping_pid ==
					    wtr) {
						printf
						    ("pinger: %s doesn't exist\n",
						     child_ping
						     [child_index].machine);
					}
				}

				return EXIT_FAILURE;
			}
		} else if (WIFSIGNALED(wstatus)) {
			printf("Killed by signal %d\n", WTERMSIG(wstatus));
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
