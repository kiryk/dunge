#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "types.h"
#include "defs.h"

List *rooms = NULL;

int main(int argc, char *argv[])
{
	pthread_t phy;

	if (argc != 2) {
		fprintf(stderr, "usage: %s port\n", argv[0]);
		exit(1);
	}
	rooms = generate();
	pthread_create(&phy, NULL, physics, (void*) &rooms);
	welcome(argv[1]);
	exit(0);
}
