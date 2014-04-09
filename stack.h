#ifndef B98TOC_STACK_H
#define B98TOC_STACK_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

Stack* stackNew() {
    Stack* ret = malloc(sizeof(*ret));
    ret->top = NULL;
    return ret;
}

void stackPush(Stack* stack, int data) {
    Node* node = malloc(sizeof(*node));
    node->data = data;
    node->next = stack->top;
    stack->top = node;
}

int stackPop(Stack* stack) {
    if(stack->top == NULL) {
        return 0;
    } else {
        Node* tmp = stack->top;
        int ret = tmp->data;
        stack->top = tmp->next;
        free(tmp);
        return ret;
    }
}

void stackFree(Stack* stack) {
    while(stack->top != NULL) {
        stackPop(stack);
    }

    free(stack);
}

#endif
