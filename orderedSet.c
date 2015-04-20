#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "tree.h"
#include "orderedSet.h"

//////////////////////////////orderedSetfunctions/////////////////////////////////
/* Make a new empty set */
struct orderedSet *orderedSetCreate(void){
    struct orderedSet *set;
    set = malloc(sizeof(struct orderedSet));
    set->root = NULL;
    //set->size = 0;

    return set;
}

/* Destroy a set */
void orderedSetDestroy(struct orderedSet *set){
    treeDestroy(&set->root);
    free(set);
}


/* Insert a new element.  Has no effect if element is already present. */
void orderedSetInsert(struct orderedSet *set, const char *target){
    treeInsert(&set->root, target);
}

/* Delete an element.  Has no effect if element is not already present. */
void orderedSetDelete(struct orderedSet *set, const char *target){
    treeDelete(&set->root, target);
}

/* How many elements in this set? */
size_t orderedSetSize(const struct orderedSet *set){
    if(set->root == NULL){
        return 0;
    } else {
    return set->root->size;
    }
}


//current points to the top of subtree
//prints set in order, L->R
void predicateApply(struct orderedSet *newSet, const struct orderedSet *set, struct tree *current, int (*predicate)(void *arg, const char *), void *arg){
    if(current == NULL){
        return;
    } else{
        if(current->child[0] == NULL && current->child[1] == NULL){
            if((*predicate)(arg, current->string)){
                orderedSetInsert(newSet, current->string);
            }
        }
        else if(current->child[0] != NULL && current->child[1] != NULL){
            predicateApply(newSet, set, current->child[0], predicate, arg);
            if((*predicate)(arg, current->string)){
                orderedSetInsert(newSet, current->string);
            }
            predicateApply(newSet, set, current->child[1], predicate, arg);
        }
        else if(current->child[0] != NULL && current->child[1] == NULL){
            predicateApply(newSet, set, current->child[0], predicate, arg);
            if((*predicate)(arg, current->string)){
                orderedSetInsert(newSet, current->string);
            }
        }
        else if(current->child[0] == NULL && current->child[1] != NULL){
            if((*predicate)(arg, current->string)){
                orderedSetInsert(newSet, current->string);
            }
            predicateApply(newSet, set, current->child[1], predicate, arg);
        }
    }
    //printf("Exit printSet\n");
}


/* Return a new ordered set containing all elements e
 * for which predicate(arg, x) != 0.
 * The predicate function should be applied to the elements in increasing order. */
struct orderedSet *orderedSetFilter(const struct orderedSet *set, int (*predicate)(void *arg, const char *), void *arg){
    struct orderedSet *newSet;
    newSet = orderedSetCreate();

    predicateApply(newSet, set, set->root, predicate, arg);

    return newSet;
}

////////////////////////////given HEIGHT helper functions//////////////////////////
int
treeHeight(const struct tree *root)
{
    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        return root->height;
    }
}

/* recompute height from height of kids */
static int
treeComputeHeight(const struct tree *root)
{
    int childHeight;
    int maxChildHeight;
    int i;

    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        maxChildHeight = TREE_EMPTY_HEIGHT;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            childHeight = treeHeight(root->child[i]);
            if(childHeight > maxChildHeight) {
                maxChildHeight = childHeight;
            }
        }

        return maxChildHeight + 1;
    }
}

///////////////////////////given SIZE helper functions////////////////////////////

size_t
treeSize(const struct tree *root)
{
    if(root == 0) {
        return 0;
    } else {
        return root->size;
    }
} 

/* recompute size from size of kids */
static int
treeComputeSize(const struct tree *root)
{
    int size;
    int i;

    if(root == 0) {
        return 0;
    } else {
        size = 1;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            size += treeSize(root->child[i]);
        }

        return size;
    }
}


//////////////////////////////////height and size FIX function///////////////////////
/* fix aggregate data in root */
/* assumes children are correct */
static void
treeFix(struct tree *root)
{
    if(root) {
        root->height = treeComputeHeight(root);
        root->size = treeComputeSize(root);
    }
}


///////////////////////////////////REBALANCING functions////////////////////////////
/* rotate child in given direction to root */
static void
treeRotate(struct tree **root, int direction)
{
    struct tree *x;
    struct tree *y;
    struct tree *b;

    /*
     *      y           x 
     *     / \         / \
     *    x   C  <=>  A   y
     *   / \             / \
     *  A   B           B   C
     */

    y = *root;                  assert(y);
    x = y->child[direction];    assert(x);
    b = x->child[!direction];

    /* do the rotation */
    *root = x;
    x->child[!direction] = y;
    y->child[direction] = b;

    /* fix y then x */
    treeFix(y);
    treeFix(x);
}

/* restore AVL property at *root after an insertion or deletion */
static void
treeRebalance(struct tree **root)
{
    int tallerKid;

    if(*root) {
        for(tallerKid = 0; tallerKid < TREE_NUM_CHILDREN; tallerKid++) {
            if(treeHeight((*root)->child[tallerKid]) >= treeHeight((*root)->child[!tallerKid]) + 2) {

                /* check if zig-zag: opposite-direction nephew is the tall one */
                /* this also covers case where both nephews are too tall */
                if(treeHeight((*root)->child[tallerKid]->child[!tallerKid]) 
                            >= treeHeight((*root)->child[tallerKid]) - 1) {
                    /* zig zag case */
                    treeRotate(&(*root)->child[tallerKid], !tallerKid);
                }

                /* fall through to zig zig case */
                treeRotate(root, tallerKid);

                /* don't bother with other kid */
                break;
            }
        }
    }
}


//////////////////////////////treeDestroy///////////////////////////////////////////

/* free all elements of a tree, replacing it with TREE_EMPTY */
void 
treeDestroy(struct tree **root)
{
    int i;

    if(*root) {
        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeDestroy(&(*root)->child[i]);
        }
        free((*root)->string);
        free(*root);
        *root = 0;
    }
}



///////////////////////////////treeInsert & treeDelete///////////////////////////////
/* insert an element into a tree pointed to by root */
void
treeInsert(struct tree **root, const char *newElement)
{
    struct tree *e;
    int dir;

    if(*root == 0) {
        /* not already there, put it in */

        //e = malloc(sizeof(*e));
        e = malloc(sizeof(struct tree));
        assert(e);

        char *new_string;
        new_string = malloc(sizeof(char) * (strlen(newElement) + 1));
        assert(new_string);

        strcpy((char *)new_string, (char *)newElement);

        e->string = new_string;
        e->child[LEFT] = 0;
        e->child[RIGHT] = 0;

        *root = e;
    } else {
        /* do this recursively so we can fix data on the way back out */
        dir = strcmp(newElement, (*root)->string);

        if(dir == 0){
            return;
        }

        else if(dir < 0){
            treeInsert(&(*root)->child[0], newElement);
        }

        else if(dir > 0){
            treeInsert(&(*root)->child[1], newElement);
        }
    }

    /* fix the aggregate data */
    treeFix(*root);
    treeRebalance(root);
}

/* return 1 if target is in tree, 0 otherwise */
int
treeContains(const struct tree *t, const char * target)
{
    while(t && t->string != target) {
        t = t->child[t->string < target];
    }

    return t != 0;
}

/* delete minimum element from the tree and return its string */
/* do not call this on an empty tree */
char *
treeDeleteMin(struct tree **root)
{

    //printf("treeDeleteMin was called!\n");
        
    struct tree *toFree;
    char * retval;

    assert(*root);  /* can't delete min from empty tree */

    if((*root)->child[LEFT]) {
        //printf("Look for min\n");
        /* recurse on left subtree */
        retval = treeDeleteMin(&(*root)->child[LEFT]);
    } else {
        /* delete the root */
        //printf("Found the min\n");

        char *new_string;

        new_string = malloc(sizeof(char) * (strlen((*root)->string) + 1));

        strcpy((char *)new_string, (char *)(*root)->string);

        toFree = *root;
        retval = new_string;
        *root = toFree->child[RIGHT];
        //free string
        free(toFree->string);    
        free(toFree);
    }

    /* fix the aggregate data */
    treeFix(*root);
    treeRebalance(root);

    //printf("Exit treeDeleteMin\n");

    return retval;
}

/* delete target from the tree */
/* has no effect if target is not in tree */
void 
treeDelete(struct tree **root, const char * target)
{

    //printf("treeDelete was called!\n");

    struct tree *toFree;
    int dir;

    /* do nothing if target not in tree */
    if(*root == NULL) {
        //printf("no target found\n");
        return;
    } else {
        dir = strcmp(target, (*root)->string);
        if(dir == 0) {
            //printf("Target found\n");

            if((*root)->child[RIGHT]) {
                //printf("Case1\n");
                /* replace root with min value in right subtree */
                //free the root string
	           free((*root)->string);
                (*root)->string = treeDeleteMin(&(*root)->child[RIGHT]);
            } else {
                //printf("Case2\n");
                /* patch out root */
                toFree = *root;
                *root = toFree->child[LEFT];

                free(toFree->string);      
                free(toFree);
            }
        }
        else if(dir < 0){
            //printf("Look left\n");
            treeDelete(&(*root)->child[0], target);
        }
        else if(dir > 0) {
            //printf("Look right\n");
            treeDelete(&(*root)->child[1], target);
        }
    /* fix the aggregate data */
    treeFix(*root);
    treeRebalance(root);

    //printf("Exit freeDelete\n");
    }
}


////////////////////////////////////tree PRINT functions///////////////////////////
/* how far to indent each level of the tree */
#define INDENTATION_LEVEL (2)

/* print contents of a tree, indented by depth */
static void
treePrintIndented(const struct tree *root, int depth)
{
    int i;

    if(root != 0) {
        treePrintIndented(root->child[LEFT], depth+1);

        for(i = 0; i < INDENTATION_LEVEL*depth; i++) {
            putchar(' ');
        }
        printf("%s Height: %d Size: %zu (%p)\n", root->string, root->height, root->size, (void *) root);

        treePrintIndented(root->child[RIGHT], depth+1);
    }
}

/* print the contents of a tree */
void 
treePrint(const struct tree *root)
{
    treePrintIndented(root, 0);
}

size_t
treeRank(const struct tree *t, char * target)
{
    size_t rank = 0;

    while(t && t->string != target) {
        if(t->string < target) {
            /* go right */
            /* root and left subtree are all less than target */
            rank += (1 + treeSize(t->child[LEFT]));
            t = t->child[RIGHT];
        } else {
            /* go left */
            t = t->child[LEFT];
        }
    }

    /* we must also count left subtree */
    return rank + treeSize(t->child[LEFT]);
}

const char *
treeUnrank(const struct tree *t, size_t rank)
{
    size_t leftSize;

    /* basic idea: if rank < treeSize(child[LEFT]), recurse in left child */
    /* if it's equal, return the root */
    /* else recurse in right child with rank = rank - treeSize(child[LEFT]) - 1 */
    while(rank != (leftSize = treeSize(t->child[LEFT]))) {
        if(rank < leftSize) {
            t = t->child[LEFT];
        } else {
            t = t->child[RIGHT];
            rank -= (leftSize + 1);
        }
    }

    return t->string;
}

