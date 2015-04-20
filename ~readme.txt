The code in this repository was selected from code I wrote in my Data Structures course,
during Spring 2015.  Below are descriptions of the code and the assignments they were
written for:

maze.c:
This program takes in a PGM file of a maze, and searches through the entire maze for
a cycle.  I used an array of structs that contained information about whether each node
was a wall or path, and what the parent node (from the traversal path of the search) and 
the parent index were.  I implemented recursive depth-first search to look for the cycle.
The output of the program is the same PGM file with the cycle marked, if found.  

orderedSet.c:
This code stores strings in an AVL tree, and sorts the strings in increasing order 
according to strcmp.  Starting with some sample code from class, I modified the code
so that strings could be stored, and so that an arbitrary function could be applied
to the contents of the AVL tree in increasing order.  I implemented an AVL tree instead
of the standard binary search tree to increase time efficiency of storing the strings, as
AVL trees are usually relatively balanced.

strategy.c:
This code simulates a card game of n piles, where there are two moves: deal or play.
Cards are always dealt to the nth pile.  In “play,” a card is moved from the kth pile
to the k-1 pile.  Any card moved to pile 0 is moved to trash.  The goal is for the 
minimum card in each pile to be as large as possible by the end of the game.  
I implemented a standard binary search tree to carry out this game, with a search function
to always find the minimum card.

ships.c:
This code simulates Battleship.  I implemented a hash table to hash points that contain 
ships.  Each point is a struct that contains information about the top-left coordinate
of the ship, and has a pointer to the ship itself. I used an array of linked lists to
represent my hash table; thus, each point contains a next pointer as well. I wrote functions that allow for a playing field to be initialized, ships to be placed, the field to be attacked, and the field to be destroyed. 

turing.c:
I wrote this code to simulate a classic turing machine.  I implemented a doubly-linked
list to represent the infinite tape in the machine.  
