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

	int max_buf = 1000;
	char *buffer = malloc(sizeof(char) * max_buf);
	if (buffer == NULL) {
		//Malloc failed
		printf("Failed to allocate memory");
		fclose(ifp);
		free(ifp);
		return 1;
	}

	//Get first line of input file, which is number of components
	int num_components;
	int buf_length = 0;
	char ch;
	while ((ch != '\n') && (ch != EOF)) {
		if (buf_length == max_buf) {
			max_buf *= 2;
			buffer = realloc(buffer, max_buf);
			if (buffer == NULL) {
				//Realloc failed
				printf("Failed to allocate memory");
				fclose(ifp);
				free(ifp);
				return 1;
			}
		}
		ch = getc(ifp);
		buffer[buf_length] = ch;
		buf_length++;
	}
	int i = 0;
	while (buffer[i] != '\0' && buffer[i] != '\n' && buffer[i]!=EOF) {
		if (!isdigit(buffer[i])) {
			printf(
					"Error: The first line of the config file is not an integer");
			fclose(ifp);
			free(ifp);
			return 1;
		}
		i++;
	}
	num_components = atoi(buffer);
	printf("%d", num_components);

	//Read the rest of the file

	fclose(ifp);
	free(ifp);
	return 0;
}

