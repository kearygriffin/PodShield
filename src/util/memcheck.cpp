#include <stdio.h>
#include <stdlib.h>
#include "../podshield.h"

int main(int argc, char *argv[]) {
	char progsize_s[132];
	char datasize_s[32];
	int progsize, datasize;

	fgets(progsize_s, sizeof(progsize_s), stdin);
	fgets(datasize_s, sizeof(datasize_s), stdin);

	progsize = atoi(progsize_s);
	datasize = atoi(datasize_s);

	printf("Prog: %d/%d, Datasize: %d/%d\n", progsize, FLASH_SIZE, datasize, SRAM_SIZE);
	exit(0);
}
