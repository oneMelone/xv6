#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_and_pass(int *);

int main() {
	int p[2];
	pipe(p);
	int pid = fork();
	if (pid == 0) {
		print_and_pass(p);
	} else {
		close(p[0]);
		for (int i = 2; i <= 35; i++) {
			write(p[1], &i, 4);
		}
		close(p[1]);
		wait(0);
	}
	exit(0);
}

void print_and_pass(int* port) {
	close(port[1]);
	int n;
	if (read(port[0], &n, 4) == 0) {
		close(port[0]);
		exit(0);
	}
	fprintf(1, "prime %d\n", n);
	int p[2];
	pipe(p);
	
	int pid = fork();
	if (pid  == 0) {
		// in child process
		print_and_pass(p);
	} else {
		// in current process
		close(p[0]);
		int x;
		while (read(port[0], &x, 4)) {
			if (x % n) {
				write(p[1], &x, 4);
			}
		}
		close(port[0]);
		close(p[1]);
		wait(0);
	}
	exit(0);
}
	
