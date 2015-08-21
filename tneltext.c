#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include <readline/readline.h>
#include <readline/history.h>

/* type */
#define MOVABLE 1
#define PICKABLE 2
#define CONTAINER 4

/* state */
#define LOCKED 1
#define BROKEN 2

struct tobj{
	char *name;
	char *desc;
	uint32_t type;
	uint32_t state;
	struct tobj *child;
	struct tobj *next;
};

int isvowel(char c){
	c = tolower(c);
	return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void tprint(struct tobj *t){
	int adj = 0;
	struct tobj *child = t->child;

	/* Put indefinite article "a" */
	printf("a");

	/* Adjectives based off state */
	if(t->state & BROKEN){
		printf(" broken,");
		++adj;
	}
	if(t->state & LOCKED){
		printf(" locked,");
		++adj;
	}

	/* Turn the indefinite article "a" into "an" if necessary */
	if(adj == 0 && isvowel(t->desc[0])){
		printf("n");
	}

	/* Description of object */
	printf(" %s %s.\n", t->desc, t->name);

	/* List of children */
	if(child){
		printf("There is");
		do{
			printf(!child->next && child != t->child ? " and %s %s " : " %s %s,", isvowel(child->name[0]) ? "an" : "a", child->name);
		}while((child = child->next));	
		printf("\b.\n");
	}
}

int lookf(struct tobj *t){
	tprint(t);
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

struct tobj *tfind(struct tobj *t, char *str){
	if(tstrcmp(str, t->name) == 0){
		return t;
	}
	t = t->child;
	if(t == NULL){
		return NULL;
	}
	do{
		if(tstrcmp(str, t->name) == 0){
			return t;
		}
	}while((t = t->next));

	return NULL;
}

int main()
{
	struct tobj trash = {"trashcan", "tin", 0, 0, NULL, NULL};
	struct tobj pen = {"pen", "ball-point", 0, 0, NULL, NULL};
	struct tobj desk = {"desk", "waferboard", 0, 0, &pen, &trash};
	struct tobj chair = {"chair", "wooden", 0, 0, NULL, &desk};
	struct tobj bathroom = {"bathroom", "ugly, run-down", CONTAINER, 0, &chair, NULL};
	struct tobj *cloc = &bathroom;
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
				printf(noun == cloc ? "You are in " : "You are looking at ");
				lookf(noun);
			}
			else if(tstrcmp(verbstr, "break") == 0){
				breakf(noun);
			}
			else if(tstrcmp(verbstr, "unlock") == 0){
				unlockf(noun);
			}
			else if(tstrcmp(verbstr, "lock") == 0){
				lockf(noun);
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
