#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Config.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	if (argc == 4) {
		//Check if argument is valid, https://stackoverflow.com/questions/45554639/is-there-a-way-to-check-if-a-string-can-be-a-float-in-c
		char *ptr;
		double time = strtod(argv[1], &ptr);
		if (ptr == argv[1] || ptr[strspn(ptr, " \t\r\n")] != '\0') {
			printf("ERROR: First argument should be a number");
			return 1;
		}
		char* configfile = argv[2];
		char* outfile = argv[3];
		return config(time, configfile, outfile);
	} else {
		printf("ERROR: Wrong number of arguments");
		return 1;
	}
}
