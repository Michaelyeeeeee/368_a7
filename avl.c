#include "avl.h"

/* helper: compute height per assignment spec:
 * empty tree has height -1
 */
static int height(Tnode *n)
{
    if (!n)
        return -1;
    int lh = height(n->left);
    int rh = height(n->right);
    return 1 + (lh > rh ? lh : rh);
}

/* update balance field for node (left height - right height) */
static void update_balance(Tnode *n)
{
    if (!n)
        return;
    n->balance = height(n->left) - height(n->right);
}

/* rotations (update balances for affected nodes) */
Tnode *rotate_left(Tnode *node)
{
    if (!node || !node->right)
        return node;
    Tnode *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;

    /* update balances (bottom-up) */
    update_balance(node);
    update_balance(new_root);
    return new_root;
}

Tnode *rotate_right(Tnode *node)
{
    if (!node || !node->left)
        return node;
    Tnode *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;

    /* update balances (bottom-up) */
    update_balance(node);
    update_balance(new_root);
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

/* balance node according to its balance factor */
Tnode *balance(Tnode *node)
{
    if (!node)
        return NULL;

    update_balance(node);

    if (node->balance > 1) /* left heavy */
    {
        /* if left child is right-heavy -> LR case */
        update_balance(node->left);
        if (node->left && node->left->balance < 0)
            return rotate_left_right(node);
        else
            return rotate_right(node);
    }
    else if (node->balance < -1) /* right heavy */
    {
        /* if right child is left-heavy -> RL case */
        update_balance(node->right);
        if (node->right && node->right->balance > 0)
            return rotate_right_left(node);
        else
            return rotate_left(node);
    }

    return node;
}

Tnode *create_node(int key)
{
    Tnode *node = (Tnode *)malloc(sizeof(Tnode));
    if (!node)
    {
        /* On memory allocation failure the assignment asks to print 0 and fail later;
           here we just return NULL and the caller handles it. */
        return NULL;
    }
    node->key = key;
    node->balance = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* insert: keeps duplicates by going left when key == root->key
 * signature kept as (root, node) to match existing create_avl usage
 */
Tnode *insert(Tnode *root, Tnode *node)
{
    if (!node)
        return root; /* nothing to insert */

    if (!root)
    {
        /* node becomes the new root of this subtree */
        return node;
    }

    if (node->key <= root->key)
        root->left = insert(root->left, node);
    else
        root->right = insert(root->right, node);

    /* update balance and rebalance if needed */
    update_balance(root);
    return balance(root);
}

/* remove_max: remove the maximum node in subtree rooted at n
 * Returns new subtree root after removal.
 * Sets *max_node to the removed node (the caller is responsible to free it or use its key).
 */
static Tnode *remove_max(Tnode *n, Tnode **max_node)
{
    if (!n)
        return NULL;
    if (!n->right)
    {
        /* this node is the max */
        *max_node = n;
        return n->left; /* return left child to be linked by parent */
    }
    n->right = remove_max(n->right, max_node);

    /* update balance and rebalance up the recursion */
    update_balance(n);
    return balance(n);
}

/* delete_node: delete the first node found by BST search for 'key'.
 * Returns new subtree root after deletion.
 */
Tnode *delete_node(Tnode *root, int key)
{
    if (!root)
        return NULL;

    if (key < root->key)
    {
        root->left = delete_node(root->left, key);
    }
    else if (key > root->key)
    {
        root->right = delete_node(root->right, key);
    }
    else
    {
        /* found node to delete (this is the first encountered by normal BST search) */
        if (!root->left && !root->right)
        {
            /* no children */
            free(root);
            return NULL;
        }
        else if (!root->left || !root->right)
        {
            /* one child */
            Tnode *child = root->left ? root->left : root->right;
            free(root);
            return child;
        }
        else
        {
            /* two children: replace with in-order predecessor (max in left subtree) */
            Tnode *pred = NULL;
            root->left = remove_max(root->left, &pred);
            if (pred)
            {
                /* copy key from predecessor into current node, then free predecessor */
                root->key = pred->key;
                free(pred);
            }
        }
    }

    /* update balance and rebalance on the way back up */
    update_balance(root);
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

/* creates avl tree for -b option */
int create_avl(char *inFile, AVL *avl)
{
    if (!avl)
        return 0;
    FILE *file1 = fopen(inFile, "rb");
    if (!file1)
    {
        fprintf(stdout, "-1\n");
        return -1;
    }

    int x;
    char y;
    while (fread(&x, sizeof(int), 1, file1) == 1 && fread(&y, sizeof(char), 1, file1) == 1)
    {
        if (y == 'i')
        {
            Tnode *node = create_node(x);
            if (!node)
            {
                fclose(file1);
                fprintf(stdout, "0\n");
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
            fclose(file1);
            fprintf(stdout, "0\n");
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
    FILE *file = fopen(outFile, "wb");
    if (!file)
    {
        printf("-1\n");
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

    char format = 0;
    if (node->left)
        format += 2;
    if (node->right)
        format += 1;

    int key = node->key;

    /* debug print kept from your original - you can remove if not needed */
    printf("%d %d\n", key, format);

    fwrite(&key, sizeof(int), 1, file);
    fwrite(&format, sizeof(char), 1, file);

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
        printf("0\n");
        return 0;
    }
    avl->root = NULL;

    int r = create_avl(inFile, avl);
    if (r == -1)
    {
        free(avl);
        return -0;
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

/* evaluates if BST */
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

/* checks balance of -e */
int check_balance(Tnode *root)
{
    if (!root)
        return 0;
    // check if children are balanced
    int left = check_balance(root->left);
    if (left == -1)
        return -1;
    int right = check_balance(root->right);
    if (right == -1)
        return -1;
    // unbalanced
    if (left - right > 1 || left - right < -1)
        return -1;
    // if balanced, then return height
    return (left > right ? left : right) + 1;
}

int evaluate_balanced(Tnode *root)
{
    return check_balance(root) != -1;
}

/* evaluates total output */
int evaluate(char *inFile)
{
    AVL *avl = malloc(sizeof(AVL));
    if (!avl)
    {
        printf("0\n");
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
    if (!avl->root)
    {
        printf("0,0,0\n");
        free(avl);
        fclose(file1);
        return -1;
    }
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
    int key;
    char code;
    if (fread(&key, sizeof(int), 1, fp) != 1 || fread(&code, sizeof(char), 1, fp) != 1)
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
