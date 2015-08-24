/* tneltext - a simple text adventure engine in C */
/* Written in 2015 by tnelsond tnelsond@gmail.com */
/* You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.*/

#include "tneltext.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <readline/readline.h>
#include <readline/history.h>

void tobj_set(struct tobj *t, char *art, char *name, char *desc, uint32_t type, uint32_t state, struct tobj *child, struct tobj *next){
	t->article = art;
	t->name = name;
	t->desc = desc;
	t->type = type;
	t->state = state;
	t->child = child;
	t->next = next;
}

int isvowel(char c){
	c = tolower(c);
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
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

/* Print description */
void tdesc(struct tobj *t, int which){
	printf("%s", t->article);

	if(which & DESC){
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
		if(adj == 0 && tstrcmp(t->article, "a") == 0 && isvowel(t->desc[0])){
			printf("n");
		}
		/* Description of object */
		printf(" %s %s.\n", t->desc, t->name);
	}
	else{
		if(isvowel(t->name[0]) && tstrcmp(t->article, "a") == 0){
			printf("n");
		}
		printf(" %s; ", t->name);
	}
}

void tprintsiblings(struct tobj *t, char *intro){
	int count = 0;
	if(t->type & INVISIBLE){
		return;
	}
	printf("%s", intro);
	do{
		if(t->type & INVISIBLE){
			break;
		}
		if(count && (!t->next || t->next->type & INVISIBLE)){
			printf("and ");
		}
		tdesc(t, t->state);
		++count;
	}while((t = t->next));	
	printf("\b\b.\n");
}

void tprint(struct tobj *t, int printself, char *intro){
	/* Print itself */
	if(printself){
		tdesc(t, DESC);
	}
	
	/* Print list of children */
	if(t->child && t->type ^ PORTAL){
		tprintsiblings(t->child, intro);
	}
	
	if(t->type & ROOM && t->next){
		tprintsiblings(t->next, "Exits are: ");
	}
}

void lookf(struct tobj *t){
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
}

void breakf(struct tobj *t){
	t->state |= BROKEN;
	printf("You broke the %s.\n", t->name);	
}


void unlockf(struct tobj *t){
	if(t->state & LOCKED){
		t->state &= ~LOCKED;
		printf("You unlocked the %s.\n", t->name);	
	}
	else{
		printf("It's already unlocked!\n");
	}
}

void gof(struct tobj *t){
	if(t->type & ROOM){
		self.next = t;
		lookf(self.next);
	}
	else if(t->type & PORTAL && t->child && t->child->type & ROOM){
		self.next = t->child;
		lookf(self.next);
	}
	else{
		printf("You can't go there!\n");
	}
}

void lockf(struct tobj *t){
	t->state |= LOCKED;
	printf("You locked the %s.\n", t->name);	
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
		if(t->type ^ PORTAL){ /* Don't look for stuff that's through a portal */
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

void eatf(struct tobj *prev, struct tobj *t){
	if(t->type & EDIBLE){
		movef(prev, t, &nothing);
		printf("You ate the %s.\n", t->name);
	}
	else{
		printf("You can't eat that!\n");
	}
}

void talkf(struct tobj *t){
	struct tobj *ct = t->child;
	if(ct){
		do{
			if(ct->type & SPEECH && ct->state ^ S_DONE){
				printf("The %s says: ``%s''\n", t->name, ct->desc);
				if(ct->type & S_ONCE){
					ct->state |= S_DONE;
				}
				break;
			}
		}while((ct = ct->next));
	}
}

void pickf(struct tobj *prev, struct tobj *t){
	if(t->type & PICKABLE){
		movef(prev, t, &self);
		printf("You picked up the %s.\n", t->name);
	}
	else{
		printf("You can't pick that up!\n");
	}
}

void dropf(struct tobj *prev, struct tobj *t){
	if(t->type & PICKABLE){
		movef(prev, t, self.next);
		printf("You dropped the %s.\n", t->name);
	}
	else{
		printf("You can't drop that!\n");
	}
}

void tloop(int history)
{
	using_history();
	stifle_history(history);

	lookf(self.next);

	while(1){
		int fail = 0;
		struct tobj *prev = NULL;
		struct tobj *noun = NULL;
		char *input = readline(">> ");
		char *verbstr = input;
		char *nounstr = verbstr;
		add_history(verbstr);

		/* Blank out spaces and punctuation and set the noun as the last word */
		while(*input){
			if(!isalpha(*input)){
				*input = '\0';
				if(isalpha(*(input + 1))){
					nounstr = input + 1;
				}
			}
			++input;
		}
		input = verbstr; /* Restore input back to where it was */

		/* Do basic substition for shortcut commands */
		if(tstrequals(verbstr, 2, "n", "north")){
			verbstr = "go";
			nounstr = "north";
		}
		else if(tstrequals(verbstr, 2, "ne", "northeast")){
			verbstr = "go";
			nounstr = "northeast";
		}
		else if(tstrequals(verbstr, 2, "e", "east")){
			verbstr = "go";
			nounstr = "east";
		}
		else if(tstrequals(verbstr, 2, "se", "southeast")){
			verbstr = "go";
			nounstr = "southeast";
		}
		else if(tstrequals(verbstr, 2, "s", "south")){
			verbstr = "go";
			nounstr = "south";
		}
		else if(tstrequals(verbstr, 2, "sw", "southwest")){
			verbstr = "go";
			nounstr = "southwest";
		}
		else if(tstrequals(verbstr, 2, "w", "west")){
			verbstr = "go";
			nounstr = "west";
		}
		else if(tstrequals(verbstr, 2, "nw", "northwest")){
			verbstr = "go";
			nounstr = "northwest";
		}
		else if(tstrequals(verbstr, 1, "away")){
			verbstr = "go";
			nounstr = "away";
		}

		noun = tfind(&self, &prev, nounstr);
		if(noun == NULL){
			if(verbstr != nounstr){
				if(tstrequals(verbstr, 2, "go", "travel")){
					printf("You can't go ``%s'' here.\n", nounstr);
				}
				else{
					printf("You don't see ``%s'' here.\n", nounstr);
				}
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
			else if(tstrequals(verbstr, 3, "drop", "leave", "abandon")){
				dropf(prev, noun);
			}
			else if(tstrequals(verbstr, 3, "talk", "say", "speak")){
				talkf(noun);
			}
			else if(tstrequals(verbstr, 6, "break", "smash", "kick", "punch", "headbutt", "kill")){
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
		free(input);
	}
}
