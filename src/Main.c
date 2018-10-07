/*
 * Main.c
 *
 *  Created on: 7 okt. 2018
 *      Author: axel_
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc == 4) {
		float time = argv[1];
		char* config = argv[2];
		char* outfile = argv[3];
	} else {
		printf("Wrong number of arguments");
	}
}
