#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
	//Get first line of input file, which is number of vertices
	int num_components;
	if (fgets(c, sizeof c, ifp) != NULL) {
		//Checks that first line is an integer
		if (!isdigit(c)) {
			return config_error();
		}
		num_components = atoi(c);
	}

	//Read the rest of the file
	while (fgets(c, sizeof c, ifp) != NULL) {

	}

	fclose(ifp);
	return 0;
}

int config_error() {
	printf("There was an error in the config file.");
	return 1;
}

