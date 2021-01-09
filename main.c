#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "trusted_cr.h"
#include "big_initialized_data.h"

extern char big_data_buffer[];

struct timeval  start_time, end_time;

int main() {
	printf("SRAM Swapping test\n");

	int TEST_BUFFER_SIZE = NUM_OF_PAGES * SMALL_PAGE_SIZE;

	int x = 0;
	
	gettimeofday(&start_time, NULL);
	for(int i = 0; i < TEST_BUFFER_SIZE; i++) {
		// Code optimization is disabled with -O0 to prevent code being optimized away.
		x = big_data_buffer[i];
	}
	gettimeofday(&end_time, NULL);
	printf ("Preparing the shared buffers: Total time = %f seconds \n", 
	(double) (end_time.tv_sec - start_time.tv_sec) + ((double) (end_time.tv_usec - start_time.tv_usec) / 1000000.0));
	
	x = 0;
	trusted_cr_migrate_to_sw();
	for(int i = 0; i < TEST_BUFFER_SIZE; i++) {
		// Code optimization is disabled with -O0 to prevent code being optimized away.
		x = big_data_buffer[i];
	}
	trusted_cr_migrate_back_to_nw();
	
	return 0;
}
