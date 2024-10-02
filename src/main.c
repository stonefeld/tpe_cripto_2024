#include <stdio.h>

int main(int argc, char **argv) {
	printf("There are %d arguments\n", argc);

	for (int i = 0; i < argc; i++)
		printf("Argument %d: %s\n", i, argv[i]);

	return 0;
}
