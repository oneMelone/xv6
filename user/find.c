#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* filename(char* path) {
	char* p = path;
	for (p = path + strlen(path); p >= path && *p != '/'; p--);

	p++;
	return p;
}

void find(char* path, char* target) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if ((fd = open(path, 0)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type) {
	case T_FILE:
		if (strcmp(filename(path), target) == 0) {
			fprintf(1, "%s\n", path);
		}
		close(fd);
		break;
	case T_DIR:
		if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
			fprintf(2, "find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';
		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
				continue;
			memcpy(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;
			find(buf, target);
		}
		close(fd);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(2, "find need 2 args");
		exit(1);
	}

	find(argv[1], argv[2]);
	exit(0);
}
