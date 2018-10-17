#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Config.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	if (argc == 4) {
		double time = strtod(argv[1],(char**)NULL);
		char* configfile = argv[2];
		char* outfile = argv[3];
		return config(time, configfile, outfile);
	} else {
		printf("Wrong number of arguments");
		return 1;
	}
}
