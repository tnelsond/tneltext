#include "tneltext.h"

#define portal(art, name, desc, from, to) \
	do{\
	struct tobj from##to = {art, name, desc, PORTAL, 0, &to, from.next};\
	from.next = &from##to;\
	}while(0)

int main(int argc, char **argv){
	tinit();

	tobj_set(&self, "your", "self", "\b", CONTAINER, 0, NULL, NULL);
	tobj_set(&nothing, "", "", "\b", CONTAINER, 0, NULL, NULL);
/* {name, desc, type, state, child, next} */
 	struct tobj brokenleg = {"your", "leg", "\b\b", 0, DESC | BROKEN | LOCKED, NULL, NULL};
	struct tobj broccoli = {PLURAL, "broccoli", "yummy", EDIBLE | PICKABLE, 0, NULL, &brokenleg};
	self.child = &broccoli;
	struct tobj girlconv2 = {NULL, "leave", "Leave me alone.", SPEECH | INVISIBLE, 0, NULL, NULL};
	struct tobj girlconv = {NULL, "hello", "Hello, creepy.", SPEECH | INVISIBLE | S_ONCE, 0, NULL, &girlconv2};
	struct tobj girl = {SINGULAR, "girl", "young", 0, 0, &girlconv, NULL};
	struct tobj trash = {SINGULAR, "trashcan", "tin", CONTAINER | PICKABLE, 0, NULL, NULL};
	struct tobj pen = {SINGULAR, "pen", "ball-point", PICKABLE, 0, NULL, NULL};
	struct tobj desk = {SINGULAR, "desk", "waferboard", CONTAINER | ROOM, 0, &pen, NULL};
	struct tobj out2desk = {SINGULAR, "desk", "waferboard", PORTAL, 0, &desk, &trash};
	struct tobj desk2out = {OWDESC, "out", "way", PORTAL, 0, NULL, NULL};
	struct tobj chair = {SINGULAR, "chair", "wooden", 0, 0, NULL, &out2desk};

	/* Rooms */
	struct tobj bathroom = {SINGULAR, "bathroom", "ugly, run-down", CONTAINER | ROOM | IN, 0, &chair, NULL};
	desk2out.child = &bathroom;
	desk.next = &desk2out;
	struct tobj kitchen = {SINGULAR, "kitchen", "smelly", CONTAINER | ROOM | IN, 0, NULL, NULL};
	struct tobj hallway = {SINGULAR, "hallway", "dim, musty", CONTAINER | ROOM | IN, 0, NULL, NULL};
	struct tobj bedroom = {SINGULAR, "bedroom", "cold, bare", CONTAINER | ROOM | IN, 0, &girl, NULL};

	/* Portals */
	portal(OWDESC, "north", "door to the", bathroom, hallway);
	portal(OWDESC, "south", "door to the", hallway, bathroom);
	portal(OWDESC, "north", "opening to the", hallway, kitchen);
	portal(OWDESC, "south", "opening to the", kitchen, hallway);
	portal(OWDESC, "east", "door to the", hallway, bedroom);
	portal(OWDESC, "west", "door to the", bedroom, hallway);
	
	/* Set starting room */
	self.next = &bathroom;

	tloop(20);	
	return 0;
} 
