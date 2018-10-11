#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Config.h"

/*
 * Runs simulation
 */
int config(float time, char* configfile, char* outfile) {
	FILE* ifp;
	ifp = fopen(configfile, "r");
	if (ifp == NULL) {
		printf("Could not open file");
		return 1;
	}
	char c[1000];
	//Get first line of input file, which is number of components
	int num_components;
	if (fgets(c, sizeof c, ifp) != NULL) {
		//Checks that first line is an integer
		int i = 0;
		while (c[i] != '\0') {
			//TODO: Check why this doesn't work
			printf("i: %d, c: %c\n", i, c[i]);
			if (!isdigit(c[i])) {
				printf(
						"Error: The first line of the config file is not an integer");
				fclose(ifp);
				return 1;
			}
			i++;
		}
		num_components = atoi(c);
	}

	//Read the rest of the file
	while (fgets(c, sizeof c, ifp) != NULL) {

	}

	fclose(ifp);
	return 0;
}

