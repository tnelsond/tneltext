#include "tneltext.h"

int main(int argc, char **argv){
	 tobj_set(&self, "your", "self", "\b", CONTAINER, 0, NULL, NULL);
	 tobj_set(&nothing, "", "", "\b", CONTAINER, 0, NULL, NULL);
/* {name, desc, type, state, child, next} */
	struct tobj brokenleg = {"a", "leg", "\b\b", 0, DESC | BROKEN | LOCKED, NULL, NULL};
	struct tobj broccoli = {"some", "broccoli", "yummy", EDIBLE | PICKABLE, 0, NULL, &brokenleg};
	self.child = &broccoli;
	struct tobj girlconv2 = {"\b", "leave", "Leave me alone.", SPEECH | INVISIBLE, 0, NULL, NULL};
	struct tobj girlconv = {"\b", "hello", "Hello, creepy.", SPEECH | INVISIBLE | S_ONCE, 0, NULL, &girlconv2};
	struct tobj girl = {"a", "girl", "young", 0, 0, &girlconv, NULL};
	struct tobj trash = {"a", "trashcan", "tin", CONTAINER | PICKABLE, 0, NULL, NULL};
	struct tobj pen = {"a", "pen", "ball-point", PICKABLE, 0, NULL, NULL};
	struct tobj desk = {"a", "desk", "waferboard", CONTAINER | ROOM, 0, &pen, NULL};
	struct tobj out2desk = {"a", "desk", "waferboard", PORTAL, 0, &desk, &trash};
	struct tobj desk2out = {"\b", "away", "", PORTAL, 0, NULL, NULL};
	struct tobj chair = {"a", "chair", "wooden", 0, 0, NULL, &out2desk};

	/* Rooms */
	struct tobj bathroom = {"a", "bathroom", "ugly, run-down", CONTAINER | ROOM | IN, 0, &chair, NULL};
	desk2out.child = &bathroom;
	desk.next = &desk2out;
	struct tobj kitchen = {"a", "kitchen", "smelly", CONTAINER | ROOM | IN, 0, NULL, NULL};
	struct tobj hallway = {"a", "hallway", "dim, musty", CONTAINER | ROOM | IN, 0, NULL, NULL};
	struct tobj bedroom = {"a", "bedroom", "cold, bare", CONTAINER | ROOM | IN, 0, &girl, NULL};

	/* Portals */
	struct tobj bath2hall = {"\b", "north", "door to the", PORTAL, 0, &hallway, NULL};
	struct tobj hall2bath = {"\b", "south", "door to the", PORTAL, 0, &bathroom, NULL};
	struct tobj hall2kitch = {"\b", "north", "opening to the", PORTAL, 0, &kitchen, &hall2bath};
	struct tobj hall2bedroom = {"\b", "east", "door to the", PORTAL, 0, &bedroom, &hall2kitch};
	struct tobj kitch2hall = {"\b", "south", "opening to the", PORTAL, 0, &hallway, NULL};
	struct tobj bed2hall = {"\b", "west", "door to the", PORTAL, 0, &hallway, NULL};
	bathroom.next = &bath2hall;
	kitchen.next = &kitch2hall;
	hallway.next = &hall2bedroom;
	bedroom.next = &bed2hall;

	self.next = &bathroom;

	tloop(20);	
	return 0;
} 
