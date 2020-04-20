#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"

int main()
{
	//mmap to get page
	if(init())
		return 1;	//mmap failed

	char *str = alloc(512);
	char *str2 = alloc(512);

	if(str == NULL || str2 == NULL)
	{
		printf("alloc failed\n");
		return 1;
	}

	strcpy(str, "Hello, world!");
	if(strcmp("Hello, world!", str))
	  printf("Hello, world! test failed\n");
	
	else
	  printf("Hello, world! test passed\n");
	
	dealloc(str);
	dealloc(str2);

	printf("Elementary tests passed\n");

	if(cleanup())
		return 1;	//munmap failed
	return 0;
}
