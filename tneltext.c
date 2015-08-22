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

/* state */
#define LOCKED 1
#define BROKEN 2
#define USEADJ 4
#define DESCR  8

struct tobj{
	char *name;
	char *desc;
	uint32_t type;
	uint32_t state;
	struct tobj *child;
	struct tobj *next;
};

struct tobj *cloc;
struct tobj self = {"self", "\b\b\b", CONTAINER, 0, NULL, NULL};

int isvowel(char c){
	c = tolower(c);
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

/* Print description */
void tdescr(struct tobj *t, int which){
	int adj = 0;
	/* Put indefinite article "a" */
	printf("a");

	if(which & USEADJ){
		/* Adjectives based off state */
		if(t->state & LOCKED){
			printf("%s locked", adj ? "," : "");
			++adj;
		}
		if(t->state & BROKEN){
			printf("%s broken", adj ? "," : "");
			++adj;
		}
	}
	
	if(which & DESCR){
		/* Turn the indefinite article "a" into "an" if necessary */
		if(adj == 0 && isvowel(t->desc[0])){
			printf("n");
		}
		/* Description of object */
		printf(" %s %s.\n", t->desc, t->name);
	}
	else{
		if(adj == 0 && isvowel(t->name[0])){
			printf("n");
		}
		printf(" %s; ", t->name);
	}
}

void tprint(struct tobj *t, int printself, char *carryingstr){
	struct tobj *child = t->child;

	/* Print itself */
	if(printself){
		tdescr(t, DESCR | USEADJ);
	}
	
	/* Print list of children */
	if(child){
		printf(carryingstr);
		do{
			if(!child->next && child != t->child){
				printf("and ");
			}
			if(child->state & USEADJ){
				tdescr(child, USEADJ);
			}
			else{
				tdescr(child, 0);
			}
		}while((child = child->next));	
		printf("\b\b.\n");
	}
}

int lookf(struct tobj *t){
	if(t == cloc){
		printf("You are in a ");
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
	t->state &= ~LOCKED;
	printf("You unlocked the %s.\n", t->name);	
	return 1;	
}

int gof(struct tobj *t){
	if(t->type & ROOM){
		cloc = t;
		lookf(t);
		return 1;
	}
	else{
		printf("You can't go in there!\n");
	}
	return -1;
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

struct tobj *tfindprev(struct tobj *src, struct tobj *target){
	src = src->child;
	if(!src || !src->child){
		return NULL;
	}
	src = src->child;
	do{
		if(src->next == target){
			return src;
		}
	}while((src = src->next));
	return NULL;
}

struct tobj *tfind(struct tobj *t, char *str){
	if(tstrcmp(str, t->name) == 0){
		return t;
	}
	else if(tstrcmp(str, "self") == 0){
		return &self;
	}
	t = t->child;
	if(!t){
		return NULL;
	}

	do{
		if(tstrcmp(str, t->name) == 0){
			return t;
		}
	}while((t = t->next));

	t = self.child;
	if(!t){
		return NULL;
	}
	do{
		if(tstrcmp(str, t->name) == 0){
			return t;
		}
	}while((t = t->next));

	return NULL;
}

void movef(struct tobj *prev, struct tobj *target, struct tobj *dest){
	if(prev){
		prev->next = target->next;
	}
	target->next = dest->child;
	dest->child = target;
}

int pickf(struct tobj *t){
	if(t->type & PICKABLE){
		/* Remove t from wherever it is and place it in the front of the inventory */
		struct tobj *temp = tfindprev(cloc, t);
		movef(temp, t, &self);
		printf("You picked up the %s.\n", t->name);
	}
	else{
		printf("You can't pick that up!\n");
		return -1;
	}
	return 0;
}

int main()
{
	struct tobj brokenleg = {"leg", "\b", 0, USEADJ | BROKEN | LOCKED, NULL, NULL};
	struct tobj broccoli = {"broccoli", "yummy", EDIBLE, 0, NULL, &brokenleg};
	self.child = &broccoli;
	struct tobj trash = {"trashcan", "tin", CONTAINER | PICKABLE, 0, NULL, NULL};
	struct tobj pen = {"pen", "ball-point", PICKABLE, 0, NULL, NULL};
	struct tobj desk = {"desk", "waferboard", CONTAINER | ROOM, 0, &pen, &trash};
	struct tobj chair = {"chair", "wooden", 0, 0, NULL, &desk};
	struct tobj bathroom = {"bathroom", "ugly, run-down", CONTAINER | ROOM, 0, &chair, NULL};
	cloc = &bathroom;
	using_history();
stifle_history(20);
	while(1){
		int fail = 0;
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

		noun = tfind(cloc, nounstr);
		if(noun == NULL){
			if(verbstr != nounstr){
				printf("I don't see ``%s'' here.\n", nounstr);
				fail = 1;
			}
			else{
				noun = cloc;
			}
		}
		if(!fail){
			if(tstrcmp(verbstr, "look") == 0){
				lookf(noun);
			}
			else if(tstrcmp(verbstr, "go") == 0){
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
			else if(tstrequals(verbstr, 2, "pick", "take")){
				pickf(noun);
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
