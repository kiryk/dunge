#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include "types.h"
#include "defs.h"
#include "dial.h"

void *physics(void *rooms)
{
	List *nxt;
	Room *r;
	Player *p;
	Item *t;

	for (;;) {
		for (r = *(Room**)rooms; r; r = (Room*) r->hdr.next) {
			pthread_mutex_lock(&r->lock);
			for (t = (Item*) r->items; t; t = (Item*) nxt) {
				nxt = t->hdr.next;
				if (prop[t->type].active)
					prop[t->type].use((Player*) r, t);
			}
			for (p = (Player*) r->plrs; p; p = (Player*) p->hdr.next) {
				pthread_mutex_lock(&p->lock);
				if (p->hunger > 0)
					p->hunger--;
				else
					p->health--;
				if (p->thirst > 0)
					p->thirst--;
				else
					p->health--;
				pthread_mutex_unlock(&p->lock);
			}
			pthread_mutex_unlock(&r->lock);
		}
		sleep(3);
	}
}

void *player(void *pp)
{
	Player *plr = (Player*) pp;
	Room *r;

	for (;;) {
		request(plr);
		if (plr->health <= 0) {
			printf("%s has died.\n", plr->hdr.name);
			r = plr->room;
			pthread_mutex_lock(&r->lock);
			join(&r->items, &plr->items);
			lfree(take(&r->plrs, plr));
			pthread_mutex_unlock(&r->lock);
			break;
		}
		answer(plr);
	}
	fclose(plr->out);
	fclose(plr->in);
	pthread_exit(0);
}

void *welcome(void *port)
{
	int i;
	FILE *in, *out;
	char name[6];
	Player *plr;
	extern List *rooms;

	for (;;) {
		host(&in, &out, port);

		do {
			fprintf(out, "nick: ");
			fflush(out);
			fscanf(in, "%5s", name);

			for (i = 0; i < 5 && name[i]; i++) {
				if (isupper(name[i]))
					name[i] = tolower(name[i]);
				else if (!isalpha(name[i])) {
					fputs("use alphabetic characters only\n\r", out);
					fflush(out);
					break;
				}
			}
		} while (name[i] != '\0');

		plr = (Player*) alloc(sizeof (Player));
		plr->room = (Room*) last(rooms);
		push(&plr->room->plrs, plr);
		plr->hdr.name = strdup(name);
		plr->hand = NULL;
		plr->in = in;
		plr->out = out;
		plr->health = MAX_HEALTH;
		plr->hunger = MAX_HUNGER;
		plr->thirst = MAX_THIRST;
		pthread_mutex_init(&plr->lock, NULL);
		printf("player %s logged in\n", plr->hdr.name);
		pthread_create(&plr->th, NULL, player, (void*) plr);
	}
}
