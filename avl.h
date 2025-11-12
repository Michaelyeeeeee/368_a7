#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hbt.h"

typedef struct AVL
{
    Tnode *root;
} AVL;

/* rotations and balancing */
Tnode *rotate_left(Tnode *node);
Tnode *rotate_right(Tnode *node);
Tnode *rotate_left_right(Tnode *node);
Tnode *rotate_right_left(Tnode *node);
Tnode *balance(Tnode *node);

Tnode *create_node(int key);

Tnode *insert(Tnode *root, Tnode *node);
Tnode *delete_node(Tnode *root, int key);

int build(char *inFile, char *outFile);
int create_avl(char *inFile, AVL *avl);
int evaluate(char *inFile);
int evaluate_bst(Tnode *root);
int check_balance(Tnode *root);
int evaluate_balanced(Tnode *root);
Tnode *buildTree(FILE *fp);
int write_tree(char *outFile, AVL *avl);
void write_node(FILE *file, Tnode *node);
void free_tree(Tnode *node);
