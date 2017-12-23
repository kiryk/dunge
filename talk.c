#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "types.h"
#include "defs.h"

static void use(Player *p, char *cmd)
{
	Item *t;

	if (t = (Item*) find(&p->items, cmd)) {
		if (!prop[t->type].active)
			prop[t->type].use(p, t);
	} else {
		if (t = (Item*) find(&p->room->items, cmd))
			if (!prop[t->type].take && !prop[t->type].active)
				prop[t->type].use(p, t);
	}
}

static void ptake(Player *p, char *cmd)
{
	Item *t;

	if (t = (Item*) find(&p->room->items, cmd))
		if (prop[t->type].take)
			push(&p->items, take(&p->room->items, t));
}

static void drop(Player *p, char *cmd)
{
	Item *t;

	if (t = (Item*) find(&p->items, cmd)) {
		if (t == p->hand)
			p->hand = NULL;
		push(&p->room->items, take(&p->items, t));
	}
}

static void attack(Player *p, char *cmd)
{
	Player *t;

	if (t = (Player*) find(&p->room->plrs, cmd))
		if (p->hand && prop[p->hand->type].attack)
			prop[p->hand->type].use(t, p->hand);
}

static void hold(Player *p, char *cmd)
{
	p->hand = (Item*) find(&p->items, cmd);
}

int request(Player *p)
{
	char cmd[BUF], *arg;

	fgets(cmd, BUF, p->in);
	if (arg = strchr(cmd, '\r'))
		*arg = '\0';
	if (arg = strchr(cmd, '\n'))
		*arg = '\0';
	if (arg = strchr(cmd, ' '))
		*arg++ = '\0';

	pthread_mutex_lock(&p->room->lock);
	if (strcmp(cmd, "exit") == 0)
		p->health = 0;
	if (arg) {
		if (strcmp(cmd, "use") == 0)
			use(p, arg);
		else if (strcmp(cmd, "attack") == 0)
			attack(p, arg);
		else if (strcmp(cmd, "take") == 0)
			ptake(p, arg);
		else if (strcmp(cmd, "drop") == 0)
			drop(p, arg);
		else if (strcmp(cmd, "hold") == 0)
			hold(p, arg);
	}
	pthread_mutex_unlock(&p->room->lock);
	return 0;
}

void answer(Player *p)
{
	FILE *f = p->out;
	Room *room = p->room;
	List *t;

	fprintf(f, "\n\rHealth: %d\tHunger: %d\tThirst: %d\n\r\n\r",
		p->health, p->hunger, p->thirst);

	fprintf(f, "Equipment:\n\r");
	for (t = p->items; t; t = t->next)
		fprintf(f, p->hand == (Item*) t?"* %s\n\r":"  %s\n\r", t->name);

	pthread_mutex_lock(&room->lock);
	fprintf(f, "\n%s:\n\r", room->hdr.name);
	for (t = room->items; t; t = t->next)
		fprintf(f, "  %s\n\r", t->name);

	fprintf(f, "\n\r");
	for (t = room->plrs; t; t = t->next)
		if ((Player*) t != p)
			fprintf(f, "  %s\n\r", t->name);
	pthread_mutex_unlock(&room->lock);
	fprintf(f, "> ");
	fflush(f);
}
