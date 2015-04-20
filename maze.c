#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
//#include <string.h>
#include <inttypes.h>

////////////////////////////////struct declarations///////////////////////////
//use an array to search through
//use getchar to read in 

struct node{
	int path;
	char mark;
	struct node *parent; //parent of traversed node
	int pindex; //index of parent
};

typedef struct node *Node;

struct maze{
	struct node **a;
	int col;
	int row;
	int size;
};

///////////////////////////DFS functions////////////////////////////////////

//global variable stop, if stop=1 stop; if stop=0 continue;
int stop = 0;

//checks if neighor is valid
int valid_neighbor(int index, int neighbor, struct maze *Maze){
	//out of bounds index or neighbor
	if((index < 0) || (index > Maze->size) || (neighbor < 0) || (neighbor > Maze->size)){
		return 0;
	}
	//index in left col
	else if(index%(Maze->col) == 0){
		if( (neighbor+1)%(Maze->col) == 0){
			return 0;
		}
		else{
			return 1;
		}
	}
	//index in right col
	else if((index+1)%(Maze->col) == 0){
		if(neighbor%(Maze->col)==0){
			return 0;
		}
		else{
			return 1;
		}
	}
	//index and neighbor are valid
	else {
		return 1;
	}
}

//checks if index is in range, then checks if path or wall
//if path, return 1
//if index is not in range, or is wall, return 0
int valid_index(int index, struct maze *Maze){
	if((index >= 0) && (index < Maze->size)){
		if(Maze->a[index]->path == 1){
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

//follow the parent of index backwards
void DFS_cycle(int index, int first_index, int last_index, struct maze *Maze){

	// printf("DFS_cycle was called!\n");
	// printf("index: %d\n", index);
	// printf("first_index: %d\n", first_index);
	// printf("last_index: %d\n", last_index);

	Maze->a[first_index]->mark = 'c';
	Maze->a[first_index]->path = 2;
	// printf("Marked index: %d\n", first_index);

	while(index != first_index){
		// printf("Marking index: %d\n", index);
		Maze->a[index]->mark = 'c';
		Maze->a[index]->path = 2;
		index = Maze->a[index]->pindex;
	}
	stop=1;
	// printf("Exit DFS_cycle\n");
}


void DFS_path(int index, int first_index, int last_index, struct maze *Maze){

	//printf("DFS_path was called!\n");
	// printf("Looking at index: %d\n", index);

	if(stop == 1){
		return;
	}

	else if((Maze->a[index]->mark == 'y') && (Maze->a[index]->parent->mark != 'c')){
		// printf("Found 'y'\n");
		first_index = index;
		DFS_cycle(last_index, first_index, last_index, Maze);
		return;
	} 

	else if (Maze->a[index]->mark == 'n') {
		// printf("Found 'n'\n");
		Maze->a[index]->mark = 'y';

		//iterate through incident edges, need 3 for loops
		int i;
		int j;
		int k;
		for(i=(index-(Maze->col)-1); i<=(index-(Maze->col)+1); i++){
			// printf("1 For Loop\n");
			if((valid_index(i, Maze) == 1) && (valid_neighbor(index, i, Maze))){
				if((i == Maze->a[index]->pindex) && (Maze->a[i]->mark == 'y')){
					// printf("Already traversed 1\n");
					continue;
				} else {
					// printf("Calls DFS_path from 1\n");
					Maze->a[i]->parent = Maze->a[index];
					Maze->a[i]->pindex = index;
					last_index = index;
					DFS_path(i, first_index, last_index, Maze);
				}
			}
			// printf("Encountered wall\n");
		}

		for(j=(index-1); j<=(index+1); j = j+2){
			// printf("2 For Loop\n");
			if((valid_index(j, Maze) == 1) && (valid_neighbor(index, j, Maze))){
				if((j == Maze->a[index]->pindex) && (Maze->a[j]->mark == 'y')){
					// printf("Already traversed 2\n");
					continue;
				} else {
					// printf("Calls DFS_path from 2\n");
					Maze->a[j]->parent = Maze->a[index];
					Maze->a[j]->pindex = index;
					last_index = index;
					DFS_path(j, first_index, last_index, Maze);
				}
			}
			// printf("Encountered wall\n");
		}

		for(k=(index+(Maze->col)-1); k<=(index+(Maze->col)+1); k++){
			// printf("3 For Loop\n");
			if((valid_index(k, Maze) == 1) && (valid_neighbor(index, k, Maze))){
				if((k == Maze->a[index]->pindex) && (Maze->a[k]->mark == 'y')){
					// printf("Already traversed 3\n");
					continue;
				} else {
					// printf("Calls DFS_path from 3\n");
					Maze->a[k]->parent = Maze->a[index];
					Maze->a[k]->pindex = index;
					last_index = index;
					DFS_path(k, first_index, last_index, Maze);
				}
			}
			// printf("Encountered wall\n");
		}
		return;
	}
	//in case inappropriately called on 'c'
	else{
		return;
	}
}


//returns index of first path
int find_first_path(struct maze *Maze){
	int index;
	index = 0;

	while(Maze->a[index]->path != 1){
		index++;
	}

	Maze->a[index]->pindex = index+1;  //assumes first path will be in bounds

	return index;
}

///////////////////////////////////////////////////////////////////////////
int main(int argc, char const *argv[])
{
	int col;
	int row;

	scanf("P5 %d %d 255\n", &col, &row);
	// printf("col: %d\n", col);
	// printf("row: %d\n", row);

	int size;
	size = col * row;

	struct maze *Maze;
	Maze = malloc(sizeof(struct maze));

	struct node **a;
	a = malloc(sizeof(struct node *) * size);

	Maze->a = a;
	Maze->col = col;
	Maze->row = row;
	Maze->size = size;

	int i;
	for(i=0; i<(size); i++){

		struct node *elt;
		elt = malloc(sizeof(struct node));
		elt->path = getchar();
		elt->mark = 'n';
		elt->parent = NULL;
		elt->pindex = 0;

		Maze->a[i] = elt;

		// printf("Maze->a[%d]->path: %d\n", i, Maze->a[i]->path);
		//printf("Maze->a[%d]->mark: %c\n", i, Maze->a[i]->mark);
	}

	//printf("%d\n", Maze->size);

	int index;
	index = find_first_path(Maze);

	DFS_path(index, index, index, Maze);

	//writing to stdout
	int zero;
	zero = 0;
	printf("P5 %d %d 255\n", Maze->col, Maze->row);
	for(i=0; i<size; i++){
		if(Maze->a[i]->path == 0){
			putchar(0);
		}
		if(Maze->a[i]->path == 1){
			putchar(1);
		}
		if(Maze->a[i]->path == 2){
			putchar(2);
		}
	}



	//free the maze 
	for(i=0; i<(size); i++){
		free(a[i]);
	}

	free(a);
	free(Maze);

	return 0;
}
