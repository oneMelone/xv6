#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int parse_args(char* argv[], int cur_len);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(2, "xargs need more args");
	}
	
	char* new_argv[MAXARG + 1];
	for (int i = 1; i < argc; i++) {
		if (i - 1 >= MAXARG) fprintf(2, "xargs: too many args");
		new_argv[i - 1] = argv[i];
	}

	while (parse_args(new_argv, argc - 1)) {
		int pid = fork();
		if (pid == 0) {
			exec(argv[1], new_argv);
			fprintf(2, "xargs: exec error");
			exit(1);
		}
	}

	while (wait(0) != -1);
	exit(0);
}

int parse_args(char* argv[], int cur_len) {
	char next_char;
	char* buffer = (char*)malloc(512);
	int cur_buffer_ptr = 0;
	if (read(0, &next_char, 1) == 0) return 0;
	do {
		if (next_char == '\n') {
			if (cur_buffer_ptr != 0) {
				buffer[cur_buffer_ptr] = 0;
				argv[cur_len++] = buffer;
			}
			break;
		}
		if (next_char == ' ') {
			buffer[cur_buffer_ptr] = 0;
			argv[cur_len++] = buffer;
			buffer = (char*)malloc(512);
			cur_buffer_ptr = 0;
			continue;
		}	
		buffer[cur_buffer_ptr++] = next_char;
		if (cur_buffer_ptr >= 511) {
			fprintf(2, "xargs: arg in stdin too long");
			exit(1);
		}
	} while (read(0, &next_char, 1) != 0);
	return 1;
}
