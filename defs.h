// lists.c
void *alloc(size_t s);
List *push(List **l, void *tp);
List *pop(List **l);
List *last(void *lp);
List *join(List **d, List **s);
List *find(List **l, char *name);
List *take(List **l, void *tp);
void  lfree(void *lp);

// generator.c
List *generate(void);

// routine.c
void *physics(void *rooms);
void *player(void *pp);
void *welcome(void *port);

// talk.c
int  request(Player*);
void answer(Player*);

// usage.c
extern const Prop prop[];
