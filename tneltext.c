/* tneltext - a simple text adventure engine in C */
/* Written in 2015 by tnelsond tnelsond@gmail.com */
/* You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>

#include <readline/readline.h>
#include <readline/history.h>

/* type */
#define MOVABLE 1
#define PICKABLE 2
#define CONTAINER 4
#define EDIBLE 8
#define ROOM 16
#define IN 32
#define PORTAL 64

/* state */
#define LOCKED 1
#define BROKEN 2
#define DESCR  4

struct tobj{
	char *name;
	char *desc;
	uint32_t type;
	uint32_t state;
	struct tobj *child;
	struct tobj *next;
};

struct tobj self = {"self", "\b\b\b", CONTAINER, 0, NULL, NULL};
struct tobj nothing = {"", "", CONTAINER, 0, NULL, NULL};

int isvowel(char c){
	c = tolower(c);
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

/* Print description */
void tdescr(struct tobj *t, int which){
	/* Put indefinite article "a" */
	printf("a");

	if(which & DESCR){
		int adj = 0;
		/* Adjectives based off state */
		if(t->state & LOCKED){
			printf(" locked,");
			++adj;
		}
		if(t->state & BROKEN){
			printf(" broken,");
			++adj;
		}

		/* Turn the indefinite article "a" into "an" if necessary */
		if(adj == 0 && isvowel(t->desc[0])){
			printf("n");
		}
		/* Description of object */
		printf(" %s %s.\n", t->desc, t->name);
	}
	else{
		if(isvowel(t->name[0])){
			printf("n");
		}
		printf(" %s; ", t->name);
	}
}

void tprint(struct tobj *t, int printself, char *carryingstr){
	struct tobj *child = t->child;
	struct tobj *next = t->next;

	/* Print itself */
	if(printself){
		tdescr(t, DESCR);
	}
	
	/* Print list of children */
	if(child && t->type ^ PORTAL){
		printf(carryingstr);
		do{
			if(!child->next && child != t->child){
				printf("and ");
			}
			tdescr(child, child->state);
		}while((child = child->next));	
		printf("\b\b.\n");
	}
	
	if(t->type & ROOM && next){
		printf("Exits are:");
		do{
			if(!next->next && next != t->next){
				printf("and ");
			}
			printf(" %s,", next->name);
		}while((next = next->next));
		printf("\b.\n");
	}
}

int lookf(struct tobj *t){
	if(t == self.next){
		printf(self.next->type & IN ? "You are in " : "You are at ");
	}
	else{
		printf("You are looking at ");
	}
	if(t == &self){
		tprint(t, 1, "You have ");
	}
	else{
		tprint(t, 1, "There is ");
	}
	return 1;	
}

int breakf(struct tobj *t){
	t->state |= BROKEN;
	printf("You broke the %s.\n", t->name);	
	return 1;	
}


int unlockf(struct tobj *t){
	if(t->state & LOCKED){
		t->state &= ~LOCKED;
		printf("You unlocked the %s.\n", t->name);	
		return 1;	
	}
	printf("It's already unlocked!\n");
	return 0;
}

int gof(struct tobj *t){
	if(t->type & ROOM){
		self.next = t;
		lookf(self.next);
		return 1;
	}
	else if(t->type & PORTAL){
		if(t->child && t->child->type & ROOM){
			self.next = t->child;
			lookf(self.next);
			return 1;
		}
	}
	printf("You can't go in there!\n");
	return 0;
}

int lockf(struct tobj *t){
	t->state |= LOCKED;
	printf("You locked the %s.\n", t->name);	
	return 1;	
}

int tstrcmp(char *a, char *b){
	while(*a && *b){
		int cmp = tolower(*a++) - tolower(*b++);
		if(cmp != 0){
			return cmp;
		}
	}
	return *a - *b;
}

/* Case insensitive */
/* 1 means match, 0 means no matches */
int tstrequals(char *a, int num, ...){
	va_list valist;
	va_start(valist, num);
	while(num-- > 0){
		if(tstrcmp(a, va_arg(valist, char *)) == 0){
			va_end(valist);
			return 1;
		}
	}
	va_end(valist);
	return 0;
}

/* Returns the tobj that has the same name as str and points prev to the tobj that's either a parent or an older sister node to the returned tobj */
struct tobj *tfind(struct tobj *t, struct tobj **prev, char *str){
	if(!t){
		return NULL;
	}
	do{
		if(tstrcmp(str, t->name) == 0){
			return t;
		}
		*prev = t;
		struct tobj *ct = t->child;
		if(ct){
			do{
				if(tstrcmp(str, ct->name) == 0){
					return ct;
				}
				*prev = ct;
				ct = ct->next;
			}while(ct);
		}
		*prev = t;
		t = t->next;	
	}while(t);

	return NULL;
}

void movef(struct tobj *prev, struct tobj *target, struct tobj *dest){
	if(prev){
		if(prev->next == target){
			prev->next = target->next;
		}
		else{
			prev->child = target->next;
		}
	}
	target->next = dest->child;
	dest->child = target;
}

int eatf(struct tobj *prev, struct tobj *t){
	if(t->type & EDIBLE){
		movef(prev, t, &nothing);
		printf("You ate the %s.\n", t->name);
		return 1;
	}
	printf("You can't eat that!\n");
	return 0;
}

int pickf(struct tobj *prev, struct tobj *t){
	if(t->type & PICKABLE){
		movef(prev, t, &self);
		printf("You picked up the %s.\n", t->name);
		return 1;
	}
	printf("You can't pick that up!\n");
	return 0;
}

int main()
{
	/* {name, descr, type, state, child, next} */
	struct tobj air = {"air", "invisible", 0, 0, NULL, NULL};
	struct tobj brokenleg = {"leg", "\b\b", 0, DESCR | BROKEN | LOCKED, NULL, NULL};
	struct tobj broccoli = {"broccoli", "yummy", EDIBLE, 0, NULL, &brokenleg};
	self.child = &broccoli;
	struct tobj trash = {"trashcan", "tin", CONTAINER | PICKABLE, 0, NULL, &air};
	struct tobj pen = {"pen", "ball-point", PICKABLE, 0, NULL, NULL};
	struct tobj desk = {"desk", "waferboard", CONTAINER | ROOM, 0, &pen, &trash};
	struct tobj chair = {"chair", "wooden", 0, 0, NULL, &desk};
	struct tobj bathroom = {"bathroom", "ugly, run-down", CONTAINER | ROOM | IN, 0, &chair, NULL};
	struct tobj kitchen = {"kitchen", "smelly", CONTAINER | ROOM | IN, 0, &trash, NULL};
	struct tobj bath2kitch = {"north", "door to the", PORTAL, 0, &kitchen, NULL};
	struct tobj kitch2bath = {"south", "door to the", PORTAL, 0, &bathroom, NULL};
	bathroom.next = &bath2kitch;
	kitchen.next = &kitch2bath;
	self.next = &bathroom;
	using_history();
stifle_history(20);
	while(1){
		int fail = 0;
		struct tobj *prev = NULL;
		struct tobj *noun = NULL;
		char *verbstr = readline(">> ");
		char *temp = verbstr;
		char *nounstr = verbstr;
		add_history(verbstr);

		/* Blank out spaces and punctuation and set the noun as the last word */
		while(*temp){
			if(!isalpha(*temp)){
				*temp = '\0';
				if(isalpha(*(temp + 1))){
					nounstr = temp + 1;
				}
			}
			++temp;
		}
		noun = tfind(&self, &prev, nounstr);
		if(noun == NULL){
			if(verbstr != nounstr){
				printf("I don't see ``%s'' here.\n", nounstr);
				fail = 1;
			}
			else{
				prev = NULL;
				noun = self.next;
			}
		}
		if(!fail){
			if(tstrcmp(verbstr, "look") == 0){
				lookf(noun);
			}
			else if(tstrequals(verbstr, 2, "i", "inventory")){
				lookf(&self);
			}
			else if(tstrequals(verbstr, 3, "eat", "consume", "ingest")){
				eatf(prev, noun);
			}
			else if(tstrequals(verbstr, 2, "go", "travel")){
				gof(noun);
			}
			else if(tstrequals(verbstr, 5, "break", "smash", "kick", "punch", "headbutt")){
				breakf(noun);
			}
			else if(tstrcmp(verbstr, "unlock") == 0){
				unlockf(noun);
			}
			else if(tstrequals(verbstr, 2, "lock", "bar")){
				lockf(noun);
			}
			else if(tstrequals(verbstr, 5, "pick", "get", "take", "steal", "pack")){
				pickf(prev, noun);
			}
			else{
				printf("Don't know how to ``%s.''\n", verbstr);
				fail = 1;
			}
		}
		free(verbstr);
	}
	return 0;
}
