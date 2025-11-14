/**
 * walk_dir.c
 *
 * Applies an function which returns an int to all contents in a directory.
 * 
 * Demonstrates the Visitor design pattern.
 * Demonstrates usage of the POSIX directory API
 * Demonstrates usage of dynamically-allocated memory
 */

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int walk_dir(char* path, int flag_recursive, int (*fn)(char *));
static int print_path(char* path);
static void join_filename(char** target, char* path, char* filename);
static void _opendir_REPORT_FAILURE();


/**
 * My driver program supplies a function that prints the filename,
 * 	checks printf's return value, and returns it to the caller.
 */
int main(int argc, char** argv) {
	char* path;
	int flag_recursive;
	int (*fn)(char*);

	if(argc != 2) {
		printf("You must supply a path\n");
		return EXIT_FAILURE;
	}
	path = argv[1];
	flag_recursive = 1;
	fn = &print_path;

	walk_dir(path, flag_recursive, fn);

	return EXIT_SUCCESS;
}


static int print_path(char* path) {
	if((printf("%s\n", path)) == 0) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


/**
 * Joins the path and the filename.
 * **target_buffer is the address of the buffer that the path and filename
 * 	are combined into.
 */
static void join_filename(char** target_buffer, char* path, char* filename) {
	int nested_name_length;

	nested_name_length = strlen(path) + 1 + strlen(filename) + 1;
	*target_buffer = (char*) calloc(nested_name_length, 1);

	strcpy(*target_buffer, path);
	strcat(*target_buffer, "/");
	strcat(*target_buffer, filename);
}


static void _opendir_REPORT_FAILURE() {
	switch (errno) {
		case (EACCES):
			perror("Permission denied");
			break;
		case (ENOENT):
			perror("Directory does not exist");
			break;
		default:
			perror("Unknown error");
			break;
	}
}


int walk_dir(char* path, int flag_recursive, int (*fn)(char *)) {
	DIR* directory;
	struct dirent* entry;
	
	char* filename;
	int error_number;
	
	struct stat file_info;
	char* nested_name;
	

	if(flag_recursive == 0) {
		return fn(path);
	}

	if((directory = opendir(path)) == NULL) {
		_opendir_REPORT_FAILURE(errno);
		return EXIT_FAILURE;
	}
	
	while((entry = readdir(directory)) != NULL) {
		filename = entry->d_name;

		if((strcmp(filename, ".") == 0) || 
			(strcmp(filename, "..") == 0)) {
			continue;
		}

		join_filename(&nested_name, path, filename);
		
		if (stat(nested_name, &file_info) == -1) {
			perror("Error retrieving file status");
			return EXIT_FAILURE;
		}

		if (S_ISREG(file_info.st_mode)) {
			error_number = walk_dir(nested_name, 0, fn);
		}
		else if (S_ISDIR(file_info.st_mode)) {
			error_number = walk_dir(nested_name, 1, fn);
		}

		free((void*) nested_name);

		if(error_number) {
			printf("Error number [%d] received from %s\n", 
				error_number, filename);
			perror("");
			return EXIT_FAILURE;
		}
	}


	/* Returns nonzero on failure */
	if(closedir(directory)) {
		printf("Error closing directory");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
