#include <stdlib.h>
#include <stdio.h>

struct node {
	struct node *next;
	struct node *prev;
	char val; /*character on the tape*/
};

typedef struct node *Node;

//just need to check if pointing to NULL to determine if you are at the
//head or tali of the doubly linked list


void freeDlist(Node head){

	if(head == NULL){

		//printf("freeing list\n");
		return;
	}

	else{
	Node temp;
	temp = head->next;
	free(head);
	freeDlist(temp);
	}
}

int main(int argc, char const *argv[])
{
	//general global variables
	int counter; //keeps track of number of steps taken
	counter = 0;

	int cstate; //keeps track of current state
	cstate = 1;

	/* will create a doubly linked list */

	//initial dlist, with char "a", next and previous point to NULL
	Node dlist;
	dlist = malloc(sizeof(struct node));
	dlist->next = NULL;
	dlist->prev = NULL;
	dlist->val = 'a';

	//initialize current pointer
	Node current;
	current = dlist;

	//UPDTE HEAD if a previous node is created
	//keep a head pointer for freeing purposes later
	Node head;
	head = dlist;

	//printf("counter: %d\n cstate: %d\n val: %c\n", counter, cstate, current->val);

//////////////////////////////Update the tape/////////////////////////////

	while(cstate != 0){

		//the following ints are used to access instruction j
		//from argv[i][j]
		int letterInt;
		letterInt = (int) (((int)current->val - 97 )* 3);
		//letterInt = atoi(&current->val);
		//letterInt = (int) current->val - 'a';

		int write;
		write = letterInt;

		int dir;
		dir = letterInt + 1;

		int nstate;
		nstate = letterInt + 2;

		//printf("letterInt: %d\n", letterInt); 

		//write char on current node
		current->val = argv[cstate][write];

		//printf("Write %c\n", current->val);

		//check + or -, and check if node is null
		//if node is null, create a new dlist element, set current
		//if node is not null, move and set current

		//printf("Direction: %c\n", argv[cstate][dir]);
		
		if(argv[cstate][dir] == '+'){
			if(current->next == NULL){
				Node dlist;
				dlist = malloc(sizeof(struct node));

				current->next = dlist;

				dlist->next = NULL;
				dlist->prev = current;
				dlist->val = 'a';  

				current = dlist;

				//printf("current->val: %c\n", current->val);
			}

			else{
				current = current->next;
			}

		}
		else if(argv[cstate][dir] == '-'){
			if(current->prev == NULL){
				Node dlist;
				dlist = malloc(sizeof(struct node));

				current->prev = dlist;

				dlist->next = current;
				dlist->prev = NULL;
				dlist->val = 'a';

				current = dlist;
				head = current;

				//printf("current->val: %c\n", current->val);
			}

			else{
				current = current->prev;
				head = current;
			}

		}
		

		//update cstate 
		cstate = atoi(&argv[cstate][nstate]);
		
		//cstate = argv[cstate][nstate] - '0';

		//printf("updated cstate: %d\n", cstate);


		//update counter
		counter += 1;

		//printf("Counter: %d\n", counter);
	}

////////////////////////////////////////////////////////////////////////////////////

	freeDlist(head);


	//print Final Counter
	printf("%d\n", counter);

	return 0;
}