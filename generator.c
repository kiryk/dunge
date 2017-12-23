#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "types.h"
#include "defs.h"

static Item *make_item(Room *room, int type, char *name)
{
	Item *item;

	item = (Item*) alloc(sizeof(Item));
	push((List**) &room->items, (List*) item);
	item->hdr.name = name? name : prop[type].name;
	item->type = type;
	return item;
}

Room *gen_staircase(char *name, uchar s, int d, int u, int c)
{
	int i;
	Room *room;

	room = (Room*) alloc(sizeof(Room));
	room->hdr.name = strdup(name);
	pthread_mutex_init(&room->lock, NULL);

	if (s & 2)
		make_item(room, DOOR, "stairs down")->data = d;
	if (s & 1)
		make_item(room, DOOR, "stairs up")->data = u;
	make_item(room, DOOR, "corridor door")->data = c;

	if (rand() % 3 == 0)
		make_item(room, METAL_PIPE, NULL);
	return room;
}

Room *gen_corridor(char *name, int s, int c, char *spec)
{
	Room *room;

	room = (Room*) alloc(sizeof(Room));
	room->hdr.name = strdup(name);
	pthread_mutex_init(&room->lock, NULL);

	make_item(room, DOOR, "staircase door")->data = c;

	make_item(room, DOOR, "toilet door")->data = s + 0;
	make_item(room, DOOR, spec)->data = s + 1;
	make_item(room, DOOR, "door 1")->data = s + 2;
	make_item(room, DOOR, "door 2")->data = s + 3;
	make_item(room, DOOR, "door 3")->data = s + 4;
	make_item(room, DOOR, "door 4")->data = s + 5;
	make_item(room, DOOR, "door 5")->data = s + 6;
	make_item(room, DOOR, "door 6")->data = s + 7;

	make_item(room, WATER_COOLER, NULL)->data = rand() % 14 + 1;

	if (rand() % 3 == 0)
		make_item(room, APPLE, NULL);
	return room;
}

Room *gen_office(char *name, uint s)
{
	Room *room;

	room = (Room*) alloc(sizeof(Room));
	room->hdr.name = strdup(name);
	pthread_mutex_init(&room->lock, NULL);

	make_item(room, DOOR, "corridor door")->data = s;

	if (rand() % 4 == 0)
		make_item(room, QUILL_PEN, NULL);
	if (rand() % 2 == 0)
		make_item(room, CRISPS, NULL);
	if (rand() % 2 == 0)
		make_item(room, SANDWICH, NULL);
	if (rand() % 4 == 0)
		make_item(room, COFFEE, NULL);
	if (rand() % 6 == 0)
		make_item(room, WATER, NULL);
	return room;
}

Room* gen_canteen(uint s)
{
	int i;
	Room *room;

	room = (Room*) alloc(sizeof(Room));
	room->hdr.name = strdup("canteen");
	pthread_mutex_init(&room->lock, NULL);

	make_item(room, DOOR, "corridor door")->data = s;

	if (rand() % 2 == 0)
		make_item(room, KNIFE, NULL);
	else
		make_item(room, FORK, NULL);
	if (rand() % 3 == 0)
		make_item(room, PIZZA, NULL);
	for (i = 0; i < 6; i++);
		if (rand() % 2 == 0)
			make_item(room, SANDWICH, NULL);
	return room;
}

Room* gen_toilet(uint s)
{
	int i;
	Room *room;

	room = (Room*) alloc(sizeof(Room));
	room->hdr.name = strdup("toilet");
	pthread_mutex_init(&room->lock, NULL);

	make_item(room, DOOR, "corridor door")->data = s;

	for (i = 0; i < 2; i++)
		if (rand() % 2 == 0)
			make_item(room, BANDAGE, NULL);
	return room;
}

List *generate(void)
{
	List *rooms = NULL;
	Room *room;
	int l, i;

	srand(time(NULL));

	push(&rooms, gen_staircase("7th floor", 1, 0, 1, 5));
	push(&rooms, gen_staircase("8th floor", 3, 0, 2, 6));
	push(&rooms, gen_staircase("9th floor", 3, 1, 3, 7));
	push(&rooms, gen_staircase("10th floor", 3, 2, 4, 8));
	push(&rooms, gen_staircase("11th floor", 2, 3, 0, 9));

	push(&rooms, gen_corridor("corridor on 7th floor", 10, 0, "canteen door"));
	push(&rooms, gen_corridor("corridor on 8th floor", 18, 1, "door to documentation room"));
	push(&rooms, gen_corridor("corridor on 9th floor", 26, 2, "magazine door"));
	push(&rooms, gen_corridor("corridor on 10th floor", 34, 3, "canteen door"));
	push(&rooms, gen_corridor("corridor on 11th floor", 42, 4, "door to computer room"));

	for (l = 0; l < 5; l++) {
		push(&rooms, gen_toilet(l+5));
		switch (l) {
		case 0:
			push(&rooms, gen_canteen(l+5));
			break;
		case 1:
			push(&rooms, gen_office("documentation room", l+5));
			break;
		case 2:
			push(&rooms, gen_office("magazine", l+5));
			break;
		case 3:
			push(&rooms, gen_canteen(l+5));
			break;
		case 4:
			room = gen_office("computer room", l+5);
			make_item(room, COMPUTER, NULL);
			push(&rooms, room);
			break;
		}
		for (i = 2; i < 8; i++)
			push(&rooms, gen_office("office", l+5));
	}
	return rooms;
}
