#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "ships.h"

///////////////////////////////THE FIELD//////////////////////////////////////////
struct point {
	struct point *next;  //linked list functionality
	struct position *Position;
	struct ship *Ship;
};

typedef struct point *Point;

struct field {
	struct point **field_array;  //field-array points to an array of pointers
	size_t count;
};

//fieldCreate mallocs an array, so it won't disappear
struct field *fieldCreate(void) {

	// printf("fieldCreate was called!\n");

	Point *a;
	a = malloc(sizeof(struct point) * 10007);

 	int i;
 	for(i=0; i<10007; i++){
 		//malloc the points?
 		Point new_point;
 		new_point = malloc(sizeof(struct point));
 		new_point->next = NULL;
 		new_point->Position = NULL;
 		new_point->Ship = NULL;

 		a[i] = new_point;
 	}

 	struct field *f;
 	//f = malloc(sizeof(a));
 	f = malloc(sizeof(struct field));
 	f->field_array = a;
 	f->count = 0;

 	// printf("Exit fieldCreate\n");
 	return f;
}

void chainDestroy(Point head) {
	Point temp;
	while (head) {
		//printf("Destroying chain\n");
		temp = head->next;
		free(head->Ship);
		free(head->Position);
		free(head);
		head = temp;
	}
}

//free each chain, then free the array
void fieldDestroy(struct field *f) {
	// printf("fieldDestroy was called!\n");
	int i;
	for(i=0; i<10007; i++) {
		//printf("field_array[%d]\n", i);
		chainDestroy(f->field_array[i]);
	}
	free(f->field_array);
	free(f);
	// printf("Exit fieldDestroy\n");
}

uint32_t hash(uint32_t x, uint32_t y) {
	uint32_t n;
	n = ((x+y)*(x+y+1)/2)+y;
	uint32_t m;
	m = n%10007;
	//return (n%10007);  //10007 possibilities, a[0] through a[100]
	return m;
}

///////////////////////////////PLACING SHIPS///////////////////////////////////////

//REWRITE clearIntersection - 1. locatePoint  2. clearIntersection
//locatePoint(*pointer) returns with current->Position == NULL or current->Position found
Point locatePoint(Point current, struct field *f, uint32_t x, uint32_t y) {
	// printf("locatePoint was called!\n");
	// printf("locatePoint is searching for (%u, %u)\n", x, y);

	uint32_t n;
	n = hash(x, y);

	//need pointer to current point being examined
	//initailly, pointer points to first element of chain in the hash table
	current = f->field_array[n];

	while(current->Position != NULL) {
		// printf("Looking\n");
		if(current->Position->x == x && current->Position->y == y){
			break;
		}
		//else
		current = current->next;
	}

	//testing clause below
	if(current->Position == NULL) {
		// printf("Searched entire linked list; Position is null\n");
	} else {
		// printf("Position is (%u, %u)\n", current->Position->x, current->Position->y);
	}
	// printf("Exit locatePoint\n");

	return current;
}

//clearIntersection examines one point using locatePoint
//if that point is null, then clearIntersection does nothing
//if that point was occupied, clearIntersection removes *entire* ship
void clearIntersection(struct field *f, uint32_t x, uint32_t y) {

	// printf("clearIntersection called!\n");

	Point current;
	current = NULL;
	current = locatePoint(current, f, x, y);
	//after locatePoint is called, current->Position == NULL
	//or current->Position->x = x and current->Position->y = y

	//point wasn't found, then no ship existed there
	//causes a SEGMENTATION FAULT!!!!
	if(current->Position == NULL){
		// printf("clearIntersection found unoccupied point\n");
		return;
	}

	//point was found, but no ship existed there
	else if(current->Position != NULL && current->Ship == NULL) {
		// printf("clearIntersection found previously occupied point; no ship currently\n");
		return;
	}

	//else, point was found, then set Ship to NULL
	//find location of that whole ship and set each point to NULL
	else {
		// printf("clearIntersection found occupied point\n");
		// printf("Ship name: %c, Ship direction: %d, Ship Position: (%u, %u), Ship length: %d\n", current->Ship->name, current->Ship->direction, current->Ship->topLeft.x, current->Ship->topLeft.y, current->Ship->length);
		if(current->Ship != NULL && current->Ship->direction == HORIZONTAL){
			// printf("Removing HORIZONTAL ship\n");
			uint32_t i;
			uint32_t x_coord;
			uint32_t y_coord;
			uint32_t x_length;
			x_coord = current->Ship->topLeft.x;
			y_coord = current->Ship->topLeft.y;
			x_length = current->Ship->length;

			struct ship *temp;
			temp = current->Ship;

			// printf("Removal should start at Ship topLeft=(%u, %u)\n", current->Ship->topLeft.x, current->Ship->topLeft.y);
			for(i= x_coord; 
				i<=(x_coord + (x_length-1)); 
				i++){
				current = locatePoint(current, f, i, y_coord);
				// printf("Ship name: %c, Ship direction: %d, Ship Position: (%u, %u), Ship length: %d\n", current->Ship->name, current->Ship->direction, current->Ship->topLeft.x, current->Ship->topLeft.y, current->Ship->length);		
				current->Ship = NULL;
				// printf("Set current->Ship to NULL successfully\n");
				if(i == (x_coord + (x_length -1))){
					break;
				}
			}
			free(temp);
			// printf("Removed HORIZONTAL ship\n");
		}
		else if(current->Ship != NULL && current->Ship->direction == VERTICAL) {
			// printf("Removing VERTICAL ship\n");
			uint32_t i;
			uint32_t x_coord;
			uint32_t y_coord;
			uint32_t y_length;
			x_coord = current->Ship->topLeft.x;
			y_coord = current->Ship->topLeft.y;
			y_length = current->Ship->length;

			struct ship *temp;
			temp = current->Ship;

			// printf("Removal should start at Ship topLeft=(%u, %u)\n", current->Ship->topLeft.x, current->Ship->topLeft.y);
			for(i=y_coord; 
				i<=(y_coord + (y_length-1)); 
				i++){
				current = locatePoint(current, f, x_coord, i);
				// printf("Ship name: %c, Ship direction: %d, Ship Position: (%u, %u), Ship length: %d\n", current->Ship->name, current->Ship->direction, current->Ship->topLeft.x, current->Ship->topLeft.y, current->Ship->length);		
				current->Ship = NULL;
				// printf("Set current->Ship to NULL successfully\n");
				if(i == (y_coord + (y_length-1))){
					break;
				}
			}
			free(temp);
			// printf("Removed VERTICAL ship\n");
		}	
		f->count -= 1;
	}
	// printf("Exit clearIntersection\n");
}


//get hash value, find empty spot in chain, place the ship
//must malloc some things
void hashShipPoint(struct field *f, uint32_t x, uint32_t y, struct ship s) {

	// printf("hashShipPoint was called!\n");

	uint32_t n;
	n = hash(x, y);

	//need pointer to current point being examined
	Point current;
	current = f->field_array[n];

	while(current->Position != NULL) {
		// printf("Searching\n");
		if(current->Position->x == x && current->Position->y == y){
			break;
		}
		//else
		current = current->next;
	}

	// printf("Searched for point to hash\n");

	//point wasn't found, then no ship existed there
	//update values of current pointer, add a new point to the end
	if(current->Position == NULL){
		// printf("Point was null\n");

		//CONTAINS BUGS???
		struct position *new_position;
		new_position = malloc(sizeof(struct position));
		new_position->x = x;
		new_position->y = y;

		struct ship *new_ship;
		new_ship = malloc(sizeof(struct ship));
		new_ship->topLeft = s.topLeft; //fixed?
		new_ship->direction = s.direction;
		new_ship->length = s.length;
		new_ship->name = s.name;

		current->Position = new_position;
		current->Ship = new_ship;

		// printf("New Point created at= (%u, %u)\n", current->Position->x, current->Position->y);
		// printf("Updated a null point\n");

		Point new_point;
		new_point = malloc(sizeof(struct point));
		new_point->next = NULL;
		new_point->Position = NULL;
		new_point->Ship = NULL;
		current->next = new_point; 


	}
	//else, point was found, then pointer to ship
	else {
		// printf("Point was previously occupied\n");
		struct ship *new_ship;  					//new addition
		new_ship = malloc(sizeof(struct ship));		//new addition
		new_ship->topLeft = s.topLeft;		//new addition
		new_ship->direction = s.direction;			//new addition
		new_ship->length = s.length;				//new addition
		new_ship->name = s.name;					//new addition

		current->Ship = new_ship;
	}

	// printf("Hashed (%u, %u)\n", current->Position->x, current->Position->y);
	// printf("Hashed point contains ship %c\n", current->Ship->name);
	// printf("Ship direction: %d\n", current->Ship->direction);

	// printf("Exit hashShipPoint\n");
}

/*
 * Place a ship in a field with given placement and name.
 *
 * If placement.length is less than one or greater than MAX_SHIP_LENGTH, 
 * or if some part of the ship would have a coordinate greater than COORD_MAX, 
 * or if the ship's name is NO_SHIP_NAME,
 * the function returns without placing a ship.
 *
 * Placing a new ship that intersects any previously-placed ships
 * sinks the previous ships, removing them from the field.
 */
void fieldPlaceShip(struct field *f, struct ship s) {

	// printf("fieldPlaceShip was called! ||||S||H||I||P||||\n");

	//testing DON'T PLACE SHIP
	if(s.length < 1 || s.length > MAX_SHIP_LENGTH) {
		// printf("exception1\n");
		return;
	}
	else if(s.direction == HORIZONTAL && (s.topLeft.x > COORD_MAX - (s.length -1))) {
			// printf("exception2\n");
			return;
	}
	else if(s.direction == VERTICAL && (s.topLeft.y > COORD_MAX - (s.length -1))) {
			// printf("exception3\n");
			return;
	}
	else if (s.name == NO_SHIP_NAME) {
		// printf("exception4\n");
		return;
	}

	//sink intersecting ships and place ship, hash each point ship occupies
	else {
		// printf("Ship is being placed.\n");
		if(s.direction == HORIZONTAL) {
			uint32_t i;
			for(i=s.topLeft.x; i<=(s.topLeft.x + (s.length-1)); i++) {
				// printf("ok1\n");  //BUG!!!!
				clearIntersection(f, i, s.topLeft.y);
				hashShipPoint(f, i, s.topLeft.y, s);
				if(i == (s.topLeft.x + (s.length -1))) {
					break;
				}
			}
		} 
		else if(s.direction == VERTICAL) {
			uint32_t i;
			for(i=s.topLeft.y; i<=(s.topLeft.y + (s.length-1)); i++) {
				// printf("ok2\n");
				clearIntersection(f, s.topLeft.x, i);
				hashShipPoint(f, s.topLeft.x, i, s);
				if(i == (s.topLeft.y + (s.length -1))) {
					break;
				}
			}
		}
		// printf("ok3\n");
	}

	f->count += 1;
	// printf("Current ship count: %zu\n", f->count);
}

/////////////////////////////ATTACK THE FIELD/////////////////////////////////////
/*
 * Attack!
 *
 * Drop a shell at given position.
 *
 * Returns NO_SHIP_NAME if attack misses (does not intersect any ship).
 *
 * Otherwise returns name of ship hit. 
 *
 * Hitting a ship sinks it, removing it from the field.
 */
 //3 cases: point doesn't exist yet; point exists & no ship; point & ship exist
char fieldAttack(struct field *f, struct position p) {

	// printf("fieldAttack was called!\n");

	uint32_t n;
	n = hash(p.x, p.y);

	//need pointer to current point being examined
	Point current;
	current = NULL;
	current = locatePoint(current, f, p.x, p.y);

	// printf("Searched for attack point\n");

	//null Point
	if(current->Position == NULL){
		// printf("Attack point was null\n");
		// printf("Exit fieldAttack\n");
		// printf("Ship count after fieldAttack: %zu\n", f->count);
		return NO_SHIP_NAME;
	}

	//Point exists, no ship
	else if(current->Position != NULL && current->Ship == NULL){
		// printf("Attack point contained no ship\n");
		// printf("Exit fieldAttack\n");
		// printf("Ship count after fieldAttack: %zu\n", f->count);
		return NO_SHIP_NAME;
	}

	//Point and ship exists
	else {
		// printf("Attack point contained a ship!\n");
		// printf("Ship being attacked has the following properties:\n");
		// printf("Name: %c, Direction: %d, topLeft: (%u, %u), Length: %d\n", current->Ship->name, current->Ship->direction, current->Ship->topLeft.x, current->Ship->topLeft.y, current->Ship->length);
		char name;
		name = current->Ship->name;
		clearIntersection(f, p.x, p.y); 

		// printf("Exit fieldAttack\n");
		// printf("Ship count after fieldAttack: %zu\n", f->count);
		return name;
	}
}

//////////////////////////////COUNT THE SHIPS/////////////////////////////////////
//update the struct field to include a count for ships
/*
 * Return number of ships in the field.
 */
size_t fieldCountShips(const struct field *f) {
	return f->count;
}

//////////////////////////////////////////////////////////////////////////////////

