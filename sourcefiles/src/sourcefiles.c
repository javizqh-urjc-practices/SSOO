#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

enum {
	MAX_PATH_SIZE = 4 * 1024
};

struct search_result {
	char *dir_name;
	int n_c_files;
	int n_h_files;
	int size;
};

struct search_result *new_search_result(char *dir_name);
void find_in_dir(char *dir_name, struct search_result *search_result,
		 int *status);
void print_search_results(struct search_result *search_result);
char *set_path(char *dir_path, char *new_path);
int size_of_file(char *path);

void
usage()
{
	fprintf(stderr, "Usage: sourcefiles\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char const *argv[])
{
	argc--;
	argv++;

	if (argc != 0)
		usage();

	// Declare variables
	char *path;
	int status = EXIT_SUCCESS;

	// Read line from stdin
	// Create buffer to read
	char *buffer = malloc(sizeof(char) * MAX_PATH_SIZE);

	if (buffer == NULL) {
		err(EXIT_FAILURE, "malloc failed");
	}
	memset(buffer, 0, sizeof(char) * MAX_PATH_SIZE);

	// Get line from stdin
	while (fgets(buffer, MAX_PATH_SIZE, stdin)) {
		path = strtok(buffer, "\n");
		struct search_result *search_result = new_search_result(path);

		find_in_dir(path, search_result, &status);

		print_search_results(search_result);
		free(search_result);
	}

	free(buffer);

	return status;
}

struct search_result *
new_search_result(char *dir_name)
{
	struct search_result *search_result =
	    (struct search_result *)malloc(sizeof(struct search_result));
	if (search_result == NULL) {
		err(EXIT_FAILURE, "maloc failed");
	}
	memset(search_result, 0, sizeof(struct search_result));
	search_result->dir_name = dir_name;
	search_result->n_c_files = 0;
	search_result->n_h_files = 0;
	search_result->size = 0;

	return search_result;
}

void
find_in_dir(char *dir_name, struct search_result *search_result, int *status)
{
	DIR *dir;
	char *extension_ptr;

	if ((dir = opendir(dir_name)) == NULL) {
		*status = EXIT_FAILURE;
		return;
	}
	// Iterate normal
	struct dirent *file_info;

	while ((file_info = readdir(dir)) != NULL) {
		if (file_info->d_type == DT_DIR) {
			// Is a directory
			if (strcmp(file_info->d_name, ".") == 0
			    || strcmp(file_info->d_name, "..") == 0)
				continue;
			char *new_path = set_path(dir_name, file_info->d_name);

			find_in_dir(new_path, search_result, status);
			free(new_path);
		} else if (file_info->d_type == DT_REG) {
			// Check for .c or .h
			extension_ptr = strrchr(file_info->d_name, '.');
			if (extension_ptr == NULL)
				continue;
			// Check for c|h and \0
			switch (*++extension_ptr) {
			case 'c':
				if (*++extension_ptr == '\0') {
					search_result->n_c_files++;
					char *new_path = set_path(dir_name,
								  file_info->d_name);

					search_result->size +=
					    size_of_file(new_path);
					free(new_path);
				}
				break;
			case 'h':
				if (*++extension_ptr == '\0') {
					search_result->n_h_files++;
					char *new_path = set_path(dir_name,
								  file_info->d_name);

					search_result->size +=
					    size_of_file(new_path);
					free(new_path);
				}
				break;
			default:
				break;
			}

		}
	}
	closedir(dir);
}

void
print_search_results(struct search_result *search_result)
{
	printf("%s %d %d %d\n", search_result->dir_name,
	       search_result->n_c_files, search_result->n_h_files,
	       search_result->size);
}

char *
set_path(char *dir_path, char *new_path)
{
	char *new_path_name = malloc(sizeof(char) * MAX_PATH_SIZE);

	if (new_path_name == NULL) {
		err(EXIT_FAILURE, "maloc failed");
	}
	memset(new_path_name, 0, MAX_PATH_SIZE);

	strcpy(new_path_name, dir_path);
	strcat(new_path_name, "/");
	strcat(new_path_name, new_path);

	return new_path_name;
}

int
size_of_file(char *path)
{
	struct stat buf;

	if (stat(path, &buf) != 0) {
		return 0;
	}

	return buf.st_size;
}
