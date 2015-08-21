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

void tprint(struct tobj *t){
	puts(t->desc);
}

int lookf(struct tobj *t){
	tprint(t);
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
	struct tobj chair = {"chair", "a wooden chair", 0, 0, NULL, NULL};
	struct tobj bathroom = {"bathroom", "a dirty run-down bathroom", 0, 0, &chair, NULL};
	struct tobj *cloc = &bathroom;
	using_history();
	stifle_history(20);
	while(1){
		struct tobj *noun = &cloc;
		char *verbstr = readline(">> ");
		char *temp = verbstr;
		char *nounstr = verbstr;
		add_history(verbstr);
		while(*temp){
			if(*temp == ' '){
				*temp = '\0';
				nounstr = temp + 1;
			}
			else if(ispunct(*temp)){
				*temp = '\0';	
			}
			++temp;
		}
		if(tstrcmp(nounstr, "chair") == 0){
			noun = &chair;
		}
		else if(verbstr != nounstr){
			printf("I don't see any ``%s'' here.", nounstr);
		}
		if(tstrcmp(verbstr, "look") == 0){
			lookf(noun);
		}
		else{
			printf("Don't know how to ``%s.''\n", verbstr);
		}
		free(verbstr);
	}
	return 0;
}
