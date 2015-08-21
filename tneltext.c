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
	printf("%s %s %s.\n", t->state & BROKEN ? "a broken" : isvowel(t->desc[0]) ? "an" : "a", t->desc, t->name);
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

int tstrcmp(char *a, char *b){
	while(*a && *b){
		int cmp = tolower(*a++) - tolower(*b++);
		if(cmp != 0){
			return cmp;
		}
	}
	return *a - *b;
}

int main()
{
	struct tobj chair = {"chair", "wooden", 0, 0, NULL, NULL};
	struct tobj bathroom = {"bathroom", "ugly run-down", 0, 0, &chair, NULL};
	struct tobj *cloc = &bathroom;
	using_history();
	stifle_history(20);
	while(1){
		int fail = 0;
		struct tobj *noun = cloc;
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

		if(tstrcmp(nounstr, "chair") == 0){
			noun = &chair;
		}
		else if(tstrcmp(nounstr, "bathroom") == 0){
			noun = &bathroom;
		}
		else if(verbstr != nounstr){
			printf("I don't see ``%s'' here.\n", nounstr);
			fail = 1;
		}
		if(!fail){
			if(tstrcmp(verbstr, "look") == 0){
				lookf(noun);
			}
			else if(tstrcmp(verbstr, "break") == 0){
				breakf(noun);
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
