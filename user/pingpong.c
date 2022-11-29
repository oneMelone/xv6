#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
	int parent_to_child[2];
	int child_to_parent[2];
	pipe(parent_to_child);
	pipe(child_to_parent);

	if (fork() == 0) {
		// in child process
		char buffer[1];
		read(parent_to_child[0], buffer, 1);
		int pid = getpid();
		fprintf(1, "%d: received ping\n", pid);
		write(child_to_parent[1], "\0", 1);
		exit(0);
	} else {
		// in parent process
		write(parent_to_child[1], "\0", 1);
		char buffer[1];
		read(child_to_parent[0], buffer, 1);
		int pid = getpid();
		fprintf(1, "%d: received pong\n", pid);
		exit(0);
	}
	exit(0);
}
