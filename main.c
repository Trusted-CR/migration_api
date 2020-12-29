#include <stdio.h>
#include <unistd.h>

#include "migration.h"

int main() {
	printf("Hello World\n");
	
	sleep(1);
	
	printf("This is a sample program\n");

	migrate_to_sw();

	sleep(1);
	printf("Intended to demo the binary\n");
	sleep(1);
	
	char words[50];
	scanf("%s", words); 
	printf("%s\n", words);
	
	
	sleep(1);
	
	int y = 0;
	
	for(int y = 0; y < 300; y++) {
		printf("all of this will run securely: %d\n", y);
	}
	
	sleep(1);
	migrate_back_to_nw();
	
	printf("Bye\n");
	
	return 0;
}
