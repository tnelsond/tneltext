/* tneltext - a simple text adventure engine in C */
/* Written in 2015 by tnelsond tnelsond@gmail.com */
/* You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.*/

#ifndef TNELTEXT_H
#define TNELTEXT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

/* type */
#define SPEECH 1
#define INVISIBLE 2
#define CONTAINER 4
#define EDIBLE 8
#define ROOM 16
#define IN 32
#define PORTAL 64
#define MOVABLE 128
#define PICKABLE 256

/* state */
#define LOCKED 1
#define BROKEN 2
#define DESC  4

/* SPEECH type */
#define S_ONCE 4

/* SPEECH state */
#define S_DONE 1

char *SINGULAR;
char *PLURAL;
char *OWDESC;

struct tobj{
	char *article;
	char *name;
	char *desc;
	uint32_t type;
	uint32_t state;
	struct tobj *child;
	struct tobj *next;
};

struct tobj self;
struct tobj nothing;

void tinit();
void tobj_set(struct tobj *t, char *adj, char *name, char *desc, uint32_t type, uint32_t state, struct tobj *child, struct tobj *next);
int isvowel(char c);
int tstrcmp(char *a, char *b);
int tstrequals(char *a, int num, ...);
void tdesc(struct tobj *t, int which);
void tprintsiblings(struct tobj *t, char *intro);
void tprint(struct tobj *t, int printself, char *intro);
void lookf(struct tobj *t);
void breakf(struct tobj *t);
void unlockf(struct tobj *t);
void gof(struct tobj *t);
void lockf(struct tobj *t);
struct tobj *tfind(struct tobj *t, struct tobj **prev, char *str);
void movef(struct tobj *prev, struct tobj *target, struct tobj *dest);
void eatf(struct tobj *prev, struct tobj *t);
void talkf(struct tobj *t);
void pickf(struct tobj *prev, struct tobj *t);
void dropf(struct tobj *prev, struct tobj *t);
void tloop(int history);

#endif
