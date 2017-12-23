#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "types.h"

void *alloc(size_t s)
{
	void *p;

	if (p = malloc(s)) {
		memset(p, 0, s);
		return p;
	}
	fprintf(stderr, "couldn't allocate memory\n");
	exit(1);
}

List *push(List **l, void *tp)
{
	List * t = (List*) tp;

	if (!l)
		return NULL;
	if (*l)
		(*l)->prev = t;
	t->next = *l;
	return *l = t;
}

List *pop(List **l)
{
	List *t;

	if (!l)
		return NULL;
	t = *l;
	*l = (*l)->next;
	if (*l)
		(*l)->prev = NULL;
	return t;
}

List *last(void *lp)
{
	List *l = (List*) lp;

	while (l && l->next)
		l = l->next;
	return l;
}

List *join(List **d, List **s)
{
	if (*s) {
		if (*d)
			((*d)->prev = last(*s))->next = *d;
		*d = *s;
		*s = NULL;
	}
	return *d;
}

List *find(List **l, char *name)
{
	List *t;

	for (t = *l; t; t = t->next)
		if (strcmp(t->name, name) == 0)
			break;
	return t;
}

List *take(List **l, void *tp)
{
	List *t = (List*) tp;

	if (!t)
		return NULL;
	if (t->prev)
		t->prev->next = t->next;
	else
		*l = t->next;
	if (t->next)
		t->next->prev = t->prev;
	t->next = t->prev = NULL;
	return t;
}

void lfree(void *lp)
{
	List *l = (List*) lp;

	if (l->name)
		free(l->name);
	free(l);
}
