#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "types.h"
#include "defs.h"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a < b ? b : a)

static Room *room_by_num(Room *r, int i)
{
	r = (Room*) last(r);
	while (r && i-- > 0)
		r = (Room*) r->hdr.prev;
	return r;
}

static void use_null(Player *p, Item *t)
{
	return;
}

static void use_door(Player *p, Item *t)
{
	int i;
	Room *r;

	pthread_mutex_lock(&p->lock);
	r = room_by_num(p->room, t->data);
	take(&p->room->plrs, p);
	pthread_mutex_unlock(&p->room->lock);
	pthread_mutex_lock(&r->lock);
	push(&r->plrs, p);
	pthread_mutex_unlock(&r->lock);
	p->room = r;
	pthread_mutex_unlock(&p->lock);
}

static void use_knife(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->health = MAX(p->health - 25, 0);
	pthread_mutex_unlock(&p->lock);
}

static void use_crisps(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->hunger = MIN(p->hunger + 30, MAX_HUNGER);
	free(take(&p->items, t));
	pthread_mutex_unlock(&p->lock);
}

static void use_apple(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->hunger = MIN(p->hunger + 40, MAX_HUNGER);
	free(take(&p->items, t));
	pthread_mutex_unlock(&p->lock);
}

static void use_drink(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->thirst = MIN(p->thirst + 30, MAX_THIRST);
	free(take(&p->items, t));
	pthread_mutex_unlock(&p->lock);
}

static void use_metal_pipe(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->health = MAX(p->health - 15, 0);
	pthread_mutex_unlock(&p->lock);
}

static void use_bandage(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->health = MIN(p->health + 20, MAX_HEALTH);
	free(take(&p->items, t));
	pthread_mutex_unlock(&p->lock);
}

static void use_water_cooler(Player *p, Item *t)
{
	if (t->data == 0)
		return;
	pthread_mutex_lock(&p->lock);
	p->thirst = MIN(p->thirst + 8, MAX_THIRST);
	pthread_mutex_unlock(&p->lock);
	t->data--;
	if (t->data == 0)
		t->hdr.name = "empty water cooler";
}

static void use_pizza(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->hunger = MIN(p->hunger + 70, MAX_HUNGER);
	free(take(&p->items, t));
	pthread_mutex_unlock(&p->lock);
}

static void use_fork(Player *p, Item *t)
{
	pthread_mutex_lock(&p->lock);
	p->health = MAX(p->health - 5, 0);
	pthread_mutex_unlock(&p->lock);
}

static void use_computer(Player *p, Item *t)
{
	Room *r;
	Player *tmp;

	for (r = (Room*) last(p->room); r; r = (Room*) r->hdr.prev) {
		if (r == p->room)
			continue;
		pthread_mutex_lock(&r->lock);
		for (tmp = (Player*) r->plrs; tmp; tmp = (Player*) tmp->hdr.next)
			fprintf(p->out, "%s is in %s\n\r", tmp->hdr.name, r->hdr.name);
		pthread_mutex_unlock(&r->lock);
	}
	fflush(p->out);
}

/*void use_zombie(Player *p, Item *t)
{
  Room *r = (Room*) p;
  obj_t *dst = get_adr(&room->obj, tmp);

  if (obj->data == 2) {
    obj_del(room, obj);
    return;
  }

  if (rand() % 3 > 0)
    return;
  if (dst->id == DOOR) {
    obj_move(plr = get_adr(&rstart, dst->data), room, obj);
    printf("%s moved to the %s.\n", obj->name, plr->name);
  } else if (dst->id == ZOMBIE && dst != obj && obj->data == 1) {
    dst->data = 2;
    printf("%s killed %s.\n", obj->name, dst->name);
  } else if (dst->id == ZOMBIE && dst != obj && obj->data == 0) {
    printf("%s is talking with %s.\n", obj->name, dst->name);
  }
  return;
}*/

const Prop prop[] = {
	/*name,          use               take, attack, active*/
	{"door",         use_door,         0,    0,      0},
	{"apple",        use_apple,        1,    0,      0},
	{"knife",        use_knife,        1,    1,      0},
	{"crisps",       use_crisps,       1,    0,      0},
	{"sandwich",     use_apple,        1,    0,      0},
	{"coffee",       use_drink,        1,    0,      0},
	{"water",        use_drink,        1,    0,      0},
	{"quill pen",    use_fork,         1,    1,      0},
	{"metal pipe",   use_metal_pipe,   1,    1,      0},
	{"bandage",      use_bandage,      1,    0,      0},
	{"water cooler", use_water_cooler, 0,    0,      0},
	{"pizza",        use_pizza,        1,    0,      0},
	{"fork",         use_fork,         1,    1,      0},
	{"zombie",       use_null,         0,    0,      1},
	{"computer",     use_computer,     0,    0,      0},
	{NULL,           NULL,             0,    0,      0}
};
