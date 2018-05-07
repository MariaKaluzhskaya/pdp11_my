#include "common.h"

int main(int argc, char * argv[])
{
	char * filename = NULL;
	if (argc > 1)
		filename = argv[1];
	
	test_mem();
	load_file(filename);
	run(01000);
	printf("reg[3] = %o\n", reg[3]);
	return 0;
}
