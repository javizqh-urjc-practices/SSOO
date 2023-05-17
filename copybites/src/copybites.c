#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Size = 4 Kb
ssize_t BUFFER_SIZE = 1024 * 4;	// In bytes

void usage();
int is_num(char *str);
int get_num(char *str);
int open_read_file(char *filename);
int open_write_file(char *filename);
void copy(int fd_src, int fd_dst);
void copy_l(int fd_src, int fd_dst, int bytes_limit);

int
main(int argc, char **argv)
{
	// ----------- Check arguments
	argv++;
	argc--;
	if (argc != 2 && argc != 3) {
		usage();
	} else if (argc == 3 && (!is_num(argv[2]) || get_num(argv[2]) <= 0)) {
		// Check if the third argument is a number
		usage();
	}
	// ----------- First Open file to read
	int fd_r = open_read_file(argv[0]);

	// ----------- Second Open file to write
	int fd_w = open_write_file(argv[1]);

	// ----------- Read file and write
	if (argc == 3) {
		copy_l(fd_r, fd_w, get_num(argv[2]));
	} else {
		copy(fd_r, fd_w);
	}

	// --------------- Close files and buffers
	if (close(fd_r) == 1 || close(fd_w) == 1) {
		err(EXIT_FAILURE, "close failed");
	}

	return 0;
}

void
usage()
{
	printf("usage: copybites SourceFile DestinationFile [nBytes]\n");
	exit(EXIT_FAILURE);
}

int
is_num(char *str)
{
	int str_len = strlen(str);
	int i;

	if (str[0] == '-') {
		return 0;
	}
	for (i = 1; i < str_len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return 0;
		}
	}
	return 1;
}

int
get_num(char *str)
{
	int total = 0;
	int str_len = strlen(str);
	int i;

	for (i = 0; i < str_len; i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			total = (total * 10) + (str[i] - '0');;
		}
	}
	return total;
}

int
open_read_file(char *filename)
{
	int fd;

	// If argument is - then read from terminal
	if (filename[0] == '-' && filename[1] == '\0') {
		fd = STDIN_FILENO;
	} else {
		fd = open(filename, O_RDONLY);
	}

	// Check if error occurred
	if (fd == -1) {
		err(EXIT_FAILURE, "open failed");
	}
	return fd;
}

int
open_write_file(char *filename)
{
	int fd;

	// If argument is - then write to terminal
	if (filename[0] == '-' && filename[1] == '\0') {
		fd = STDOUT_FILENO;
	} else {
		fd = open(filename, O_TRUNC | O_WRONLY);
		// Check if error occurred, file not found
		if (fd == -1) {
			fd = open(filename, O_CREAT | O_WRONLY, 0777);
		}
	}
	// Check if error occurred again
	if (fd == -1) {
		err(EXIT_FAILURE, "open failed");
	}
	return fd;
}

void
copy(int fd_src, int fd_dst)
{
	// Create Buffer
	char *buf = malloc(sizeof(char[BUFFER_SIZE]));

	// Check if malloc failed
	if (buf == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	// Initialize buffer to 0
	memset(buf, 0, BUFFER_SIZE);
	// ----------- Read file and write
	ssize_t bytes_to_w, bytes_w;
	ssize_t count = BUFFER_SIZE;

	do {
		bytes_to_w = read(fd_src, buf, count);
		bytes_w = write(fd_dst, buf, bytes_to_w);
	} while (bytes_to_w > 0 && bytes_w == bytes_to_w);
	// -----------------------------------------

	// Free buffer
	free(buf);
}

void
copy_l(int fd_src, int fd_dst, int bytes_limit)
{
	// Create Buffer
	char *buf = malloc(sizeof(char[BUFFER_SIZE]));

	// Check if malloc failed
	if (buf == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	// Initialize buffer to 0
	memset(buf, 0, BUFFER_SIZE);
	// ----------- Read file and write
	ssize_t bytes_to_w, bytes_w;
	ssize_t count = BUFFER_SIZE;
	ssize_t must_write_bytes;

	must_write_bytes = bytes_limit;

	do {
		bytes_to_w = read(fd_src, buf, count);
		if (must_write_bytes > bytes_to_w) {
			bytes_w = write(fd_dst, buf, bytes_to_w);
			must_write_bytes -= bytes_to_w;
		} else {
			bytes_w = write(fd_dst, buf, must_write_bytes);
		}
	} while (bytes_to_w > 0 && bytes_w == bytes_to_w);
	// -----------------------------------------

	// Free buffer
	free(buf);
}
