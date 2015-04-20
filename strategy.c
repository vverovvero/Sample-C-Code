#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <inttypes.h> /* for PRIx64 */
#include <limits.h>   /* for UCHAR_MAX */

#include "strategy.h"

////////////////////////////////strategyCreate//////////////////////////////////

//represents the piles as an array
//pointer to a struct strategy is called STRATEGY
struct strategy {
	struct node **piles;
	int numSize;
};

typedef struct strategy *STRATEGY;

struct node {
	struct node *left;
	struct node *right;
	Card key;
};

typedef struct node *Node;

/* set up a new strategy for numPiles many piles */
/* array has an extra +1, because a[0] is garbage collect */
/* usable indices from 1 to numPiles */
Strategy *strategyCreate(int numPiles){

	// printf("strategyCreate was called!\n");

	Node * a;
	a = malloc(sizeof(struct node *) * (numPiles + 1));

	int i;

	for(i=0; i<= numPiles; i++){
		a[i] = NULL;
	}

	STRATEGY Strategy;
	Strategy = malloc(sizeof(struct strategy));

	Strategy->piles = a;
	Strategy->numSize = numPiles;

	// printf("Exit strategyCreate\n");

	return Strategy;
}

/////////////////////////////////strategyDestroy///////////////////////////////////

//if root==NULL, then tree is empty
//else, tree isn't empty, and directly check left and right child
void nodeDestroy(Node root){
	while(root != NULL){

		if(root->left == NULL && root->right == NULL){
			break;
		}
		
		else if(root->left != NULL && root->right == NULL){
			nodeDestroy(root->left);
			break;
		}

		else if(root->left == NULL && root->right != NULL){
			nodeDestroy(root->right);
			break;
		}
		else {	
			nodeDestroy(root->left);
			nodeDestroy(root->right);
			break;
		}

	}
	free(root);

	return;
}


/* clean up all space used by a strategy */
//go into the strategy, free each tree in the array, then free array and struct strategy
void strategyDestroy(Strategy *Strategy){

	// printf("strategyDestroy was called!\n");

	int i;
	for(i=0; i<Strategy->numSize+1; i++){
		// printf("Pile: %d\n", i);
		nodeDestroy(Strategy->piles[i]);
	}

	free(Strategy->piles);
	free(Strategy);

	// printf("Exit strategyDestroy\n");
}

///////////////////////////////////insertNode/////////////////////////////////

//inserting a Card into pile k
void insertNode(Strategy *Strategy, int k, Card card){

	// printf("insertNode was called!\n");

	Node new_node;
	new_node = malloc(sizeof(struct node));
	new_node->key = card;
	new_node->left = NULL;
	new_node->right = NULL;

	//Case 1: Inserting at the root
	if(Strategy->piles[k] == NULL){

		// printf("Insert at root\n");

		Strategy->piles[k] = new_node;

		// printf("Inserted card: %llu\n", Strategy->piles[k]->key);

	}

	else {
		//Case 2: Inserting at a child

		// printf("Insert at child\n");

		Node current;
		current = Strategy->piles[k];	

		Node parent;
		parent = current;

		//direction 0 Left, 1 Right
		int direction;
		direction = 0;

		while(current != NULL){
			if(card < current->key){
				parent = current;
				current = current->left;
				direction = 0;
			} else {
				parent = current;
				current = current->right;
				direction = 1;
			}
		}

		// Node new_node;
		// new_node = malloc(sizeof(struct node));
		// new_node->key = card;
		// new_node->left = NULL;
		// new_node->right = NULL;

		//current = new_node;

		if(direction == 0){
			//parent->left = current;
			parent->left = new_node;
			// printf("Inserted card: %llu\n", parent->left->key);
		} else {
			//parent->right = current;
			parent->right = new_node;
			// printf("Inserted card: %llu\n", parent->right->key);
		}
	} 

	// printf("Exit insertNode\n");
}

///////////////////////////////////deleteNodeReturn///////////////////////////////////
//find smallest Card value
//keep searching left until you find the last thing
//assumes tree isn't empty

//returns the smallest card value, and deletes it from the tree
Card deleteNodeReturn(Strategy *Strategy, int k){

	// printf("deleteNodeReturn was called!\n");

	//Search for node to delete

	Node current;
	current = Strategy->piles[k];

	Node parent;
	parent = current;

	// printf("Pointers initialized\n");

	while(current->left != NULL){
		parent = current;
		current = current->left;
	}

	// printf("Done searching\n");

	Card card;
	card = current->key;

	//Case 1: Deleting the root
	if(parent == current){
		// printf("Delete the root\n");

		if(parent->right == NULL){
			Strategy->piles[k] = NULL;
		} else {
			Strategy->piles[k] = current->right;
		}
		// printf("Deleting card: %llu\n", current->key);
		free(current);

		// printf("Exit deleteNodeReturn\n");
		//return card;
	}

	//Case 2: Deleting a left child
	if(parent != current){
		// printf("Delete a left child\n");

		if(current->right == NULL){
			parent->left = NULL;
		} else {
			parent->left = current->right;
		}

		// printf("Deleting card: %llu\n", current->key);
		free(current);

		// printf("Exit deleteNodeReturn\n");

		//return card;
	}
	return card;
}


//////////////////////////////////strategyDeal////////////////////////////////////
/* add a card to the current hand */
//find numSize, and add Card to the tree in array[numSize]  (ok b/c 0 is garbage)
void strategyDeal(Strategy *Strategy, Card card){
	// printf("strategyDeal was called!\n");

	insertNode(Strategy, Strategy->numSize, card);

	// printf("Exit strategyDeal\n");
}


//////////////////////////////////strategyPlay/////////////////////////////////////
/* play a card from pile k */
//strategyPlay is never called on an empty pile
Card strategyPlay(Strategy *Strategy, int k){

	// printf("strategyPlay was called!\n");

	Card card;
	card = deleteNodeReturn(Strategy, k);
	//if pile is k-1, then garbarge collect the value
	//else, move card from k to k-1
	if( (k-1) == 0) {
		// printf("Garbage collect\n");
		// printf("Exit strategyPlay\n");
		return card;
	} else {
		insertNode(Strategy, k-1, card);

		// printf("Exit strategyPlay\n");
		return card;
	}
}


///////////////////////////////////////////////////////////////////////////////////
