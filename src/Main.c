#include <stdio.h>
#include <stdlib.h>
#include "Config.h"

int main(int argc, char *argv[]) {
	if (argc == 4) {
		float time = atof(argv[1]);
		char* configfile = argv[2];
		char* outfile = argv[3];
		return config(time, configfile, outfile);
	} else {
		printf("Wrong number of arguments");
		return NULL;
	}
}
