#ifndef _LINKED_BINARY_TREE_H_
#define _LINKED_BINARY_TREE_H_

struct TreeNode {
    uint64_t index;
    void * data;
    struct TreeNode * left;
    struct TreeNode * right;
    struct TreeNode * next;
    struct TreeNode * prev;
};

struct LinkedBinaryTree {
    struct TreeNode * root;
    int numNodes;
};

static TreeNode * newTreeNode(uint64_t index, void * data) {
    struct TreeNode * ret = malloc(sizeof(TreeNode));
    ret->index = index;
    ret->data = data;
    ret->left = NULL;
    ret->right = NULL;
    ret->next = NULL;
    ret->prev = NULL;
    return ret;
}

#endif
