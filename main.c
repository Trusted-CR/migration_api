#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "trusted_cr.h"
#include "big_initialized_data.h"

extern char big_data_buffer[];

struct timespec  start, end;

#define SEC_TO_NANOSEC 1000000000

int main() {
	printf("SRAM Swapping test\n");

	unsigned long long starttime;
	unsigned long long stoptime;
	unsigned long long elapsedtime;

	int TEST_BUFFER_SIZE = NUM_OF_PAGES * SMALL_PAGE_SIZE;

	int x = 0;
	int times = 0;
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(int y = 0; y < 50000; y++) {
		for(int i = 0; i < TEST_BUFFER_SIZE; i++) {
			// Code optimization is disabled with -O0 to prevent code being optimized away.
			x = big_data_buffer[i];
			times++;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	starttime = start.tv_sec * SEC_TO_NANOSEC + start.tv_nsec;
	stoptime = end.tv_sec * SEC_TO_NANOSEC + end.tv_nsec;

	elapsedtime = stoptime - starttime;	
	printf("%d times - %d seconds and %d nsec\n", times, elapsedtime / SEC_TO_NANOSEC,  elapsedtime % SEC_TO_NANOSEC);

	times = 0;
	x = 0;
	trusted_cr_migrate_to_sw();
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(int y = 0; y < 50000; y++) {
		for(int i = 0; i < TEST_BUFFER_SIZE; i++) {
			// Code optimization is disabled with -O0 to prevent code being optimized away.
			x = big_data_buffer[i];
			times++;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	starttime = start.tv_sec * SEC_TO_NANOSEC + start.tv_nsec;
	stoptime = end.tv_sec * SEC_TO_NANOSEC + end.tv_nsec;

	elapsedtime = stoptime - starttime;	
	printf("%d times - %d seconds and %d nsec\n", times, elapsedtime / SEC_TO_NANOSEC,  elapsedtime % SEC_TO_NANOSEC);
	trusted_cr_migrate_back_to_nw();
	
	return 0;
}
