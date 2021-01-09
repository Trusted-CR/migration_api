#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "trusted_cr.h"

#define NUM_OF_PAGES 50 
#define SMALL_PAGE_SIZE 4096

int main() {
	printf("SRAM Swapping test\n");

	int TEST_BUFFER_SIZE = NUM_OF_PAGES * SMALL_PAGE_SIZE;
	char * test_buffer = malloc(TEST_BUFFER_SIZE);

	if(!test_buffer) {
		printf("Exit: Unable to allocate %d bytes\n", TEST_BUFFER_SIZE);
		exit(1);
	}

	// Test touching multiple pages repeatedly assigning and reading from them causing them to get swapped in and out of SRAM
	// Use this to measure pagefault performance  
	trusted_cr_migrate_to_sw();
	for(int x = 0; x < 10000; x++) {
		for(int i = 0; i < TEST_BUFFER_SIZE; i += 4096) {
			test_buffer[i] = test_buffer[TEST_BUFFER_SIZE-i];
		}
	}
	trusted_cr_migrate_back_to_nw();

	free(test_buffer);
		
	return 0;
}
