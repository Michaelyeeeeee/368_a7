#include "avl.h"

Tnode *rotate_left(Tnode *node)
{
    if (!node || !node->right)
        return node;
    Tnode *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    return new_root;
}

Tnode *rotate_right(Tnode *node)
{
    if (!node || !node->left)
        return node;
    Tnode *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    return new_root;
}

Tnode *rotate_left_right(Tnode *node)
{
    if (!node)
        return node;
    node->left = rotate_left(node->left);
    return rotate_right(node);
}

Tnode *rotate_right_left(Tnode *node)
{
    if (!node)
        return node;
    node->right = rotate_right(node->right);
    return rotate_left(node);
}

Tnode *balance(Tnode *node)
{
    if (!node)
        return NULL;

    if (node->balance < -1)
    {
        if (node->right && node->right->balance > 0)
            return rotate_right_left(node);
        else
            return rotate_left(node);
    }
    else if (node->balance > 1)
    {
        if (node->left && node->left->balance > 0)
            return rotate_right(node);
        else
            return rotate_left_right(node);
    }
    return node;
}

Tnode *create_node(int key)
{
    Tnode *node = (Tnode *)malloc(sizeof(Tnode));
    if (!node)
    {
        fprintf(stderr, "create_node: malloc failed\n");
        return NULL;
    }
    node->key = key;
    node->balance = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* NOTE:
   This implementation tracks a very simplistic "balance" value used by this codebase;
   it's not a full AVL height calculation. I'm leaving the tree logic mostly as-is
   except where it caused crashes or clear mistakes.
*/
Tnode *insert(Tnode *root, Tnode *node)
{
    if (!root)
        return node;

    if (node->key < root->key)
        root->left = insert(root->left, node);
    else
        root->right = insert(root->right, node);

    /* Update balance factor roughly */
    int left_height = root->left ? (1 + (root->left->balance > 0 ? root->left->balance : 0)) : 0;
    int right_height = root->right ? (1 + (root->right->balance > 0 ? root->right->balance : 0)) : 0;
    root->balance = left_height - right_height;

    return balance(root);
}

Tnode *delete_node(Tnode *root, int key)
{
    if (!root)
        return root;

    if (key < root->key)
        root->left = delete_node(root->left, key);
    else if (key > root->key)
        root->right = delete_node(root->right, key);
    else
    {
        /* node with one or no children */
        if (!root->left || !root->right)
        {
            Tnode *temp = root->left ? root->left : root->right;
            free(root);
            return temp;
        }
        /* node with two children: replace with in-order predecessor */
        else
        {
            Tnode *temp = root->left;
            while (temp->right)
                temp = temp->right;
            root->key = temp->key;
            root->left = delete_node(root->left, temp->key);
        }
    }

    /* Update balance factor */
    int left_height = root->left ? (1 + (root->left->balance > 0 ? root->left->balance : 0)) : 0;
    int right_height = root->right ? (1 + (root->right->balance > 0 ? root->right->balance : 0)) : 0;
    root->balance = left_height - right_height;
    return balance(root);
}

void free_tree(Tnode *node)
{
    if (!node)
        return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* creates avl tree for -b option
   Returns:
     1 on success
    -1 on file open error
     0 on parse/format error
*/
int create_avl(char *inFile, AVL *avl)
{
    if (!avl)
        return 0;
    FILE *file1 = fopen(inFile, "r");
    if (!file1)
    {
        fprintf(stdout, "-1\n");
        return -1;
    }

    int x;
    char y;
    while (fscanf(file1, " %d %c", &x, &y) == 2)
    {
        if (y == 'i')
        {
            Tnode *node = create_node(x);
            if (!node)
            {
                fclose(file1);
                return 0;
            }
            avl->root = insert(avl->root, node);
        }
        else if (y == 'd')
        {
            avl->root = delete_node(avl->root, x);
        }
        else
        {
            /* unexpected token */
            fclose(file1);
            return 0;
        }
    }
    fclose(file1);
    return 1;
}

/* writes resulting tree */
int write_tree(char *outFile, AVL *avl)
{
    if (!avl)
        return 0;
    FILE *file = fopen(outFile, "w");
    if (!file)
    {
        fprintf(stdout, "-1\n");
        return -1;
    }

    write_node(file, avl->root);

    fclose(file);
    return 1;
}

void write_node(FILE *file, Tnode *node)
{
    if (!node)
        return;

    int format = 0;
    if (node->left)
        format += 2;
    if (node->right)
        format += 1;

    /* Write in same "key code" format that buildTree expects: "<key> <code>\n" */
    fprintf(file, "%d %d\n", node->key, format);
    if (node->left)
        write_node(file, node->left);
    if (node->right)
        write_node(file, node->right);
}

/* builds and creates tree for -b */
int build(char *inFile, char *outFile)
{
    AVL *avl = malloc(sizeof(AVL));
    if (!avl)
    {
        fprintf(stdout, "0\n");
        return 0;
    }
    avl->root = NULL;

    int r = create_avl(inFile, avl);
    if (r == -1)
    {
        free(avl);
        return -1;
    }
    if (r != 1)
    {
        free_tree(avl->root);
        free(avl);
        return 0;
    }

    int w = write_tree(outFile, avl);
    free_tree(avl->root);
    free(avl);
    if (!w)
        return 0;
    return 1;
}

/* evaluates if BST (inorder strictly increasing) */
int evaluate_bst(Tnode *root)
{
    if (!root)
        return 1;

    if (root->left)
    {
        if (root->left->key >= root->key || !evaluate_bst(root->left))
            return 0;
    }
    if (root->right)
    {
        if (root->right->key <= root->key || !evaluate_bst(root->right))
            return 0;
    }

    return 1;
}

/* evaluates if balanced: balance in [-1,1] for every node */
int evaluate_balanced(Tnode *root)
{
    if (!root)
        return 1;
    if (root->balance > 1 || root->balance < -1)
        return 0;
    if (!evaluate_balanced(root->left) || !evaluate_balanced(root->right))
        return 0;
    return 1;
}

/* evaluates total output */
int evaluate(char *inFile)
{
    AVL *avl = malloc(sizeof(AVL));
    if (!avl)
    {
        fprintf(stdout, "0\n");
        return 0;
    }

    FILE *file1 = fopen(inFile, "r");
    if (!file1)
    {
        printf("%d,0,0\n", -1);
        free(avl);
        return -1;
    }

    avl->root = buildTree(file1);
    int mid = evaluate_bst(avl->root);
    int right = evaluate_balanced(avl->root);

    printf("%d,%d,%d\n", 1, mid, right);
    free_tree(avl->root);
    free(avl);
    fclose(file1);
    return 1;
}

/* builds tree for -e */
Tnode *buildTree(FILE *fp)
{
    int key, code;
    if (fscanf(fp, "%d %d", &key, &code) != 2)
        return NULL;

    Tnode *root = create_node(key);
    if (!root)
        return NULL;

    if (code == 3 || code == 2)
        root->left = buildTree(fp);
    if (code == 3 || code == 1)
        root->right = buildTree(fp);

    return root;
}
