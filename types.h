#define BUF 128
#define LEN 32

#define MAX_HEALTH 100
#define MAX_HUNGER 100
#define MAX_THIRST 100

typedef unsigned char uchar;

enum {
  DOOR,
  APPLE,
  KNIFE,
  CRISPS,
  SANDWICH,
  COFFEE,
  WATER,
  QUILL_PEN,
  METAL_PIPE,
  BANDAGE,
  WATER_COOLER,
  PIZZA,
  FORK,
  ZOMBIE,
  COMPUTER
};

typedef struct List {
  struct List *next, *prev;
  char *name;
} List;

typedef struct {
  List hdr;
  int type;
  int data;
} Item;

typedef struct {
  List hdr;
  List *items;
  List *plrs;
  pthread_mutex_t lock;
} Room;

typedef struct {
  List hdr;
  List *items;
  Room *room;
  Item *hand;
  int health, hunger, thirst;
  FILE *in, *out;
  pthread_mutex_t lock;
  pthread_t th;
} Player;

typedef struct {
  char *name;
  void (*use)(Player*, Item*);
  uchar take:1;
  uchar attack:1;
  uchar active:1;
} Prop;
