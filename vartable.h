#ifndef B98TOC_TABLE_H
#define B98TOC_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct TableNode {
    int x, y;
    int value;

    struct TableNode* next;
} TableNode;

typedef struct {
    TableNode* top;
} Table;

Table* tableNew() {
    Table* ret = malloc(sizeof(*ret));
    ret->top = NULL;
    return ret;
}

void tablePut(Table* table, int x, int y, int value) {
    if(table->top == NULL) {
        TableNode* node = malloc(sizeof(*node));
        node->x = x;
        node->y = y;
        node->value = value;
        node->next = NULL;
        table->top = node;
    } else {
        TableNode* node = table->top;
        while(node != NULL) {
            if(node->x == x && node->y == y) {
                node->value = value;
                break;
            } else {
                node = node->next;
            }
        }
        if(node == NULL) {
            node = malloc(sizeof(*node));
            node->x = x;
            node->y = y;
            node->value = value;
            node->next = table->top;
            table->top = node;
        }
    }
}

bool tableGet(Table* table, int x, int y, int* value) {
    TableNode* node = table->top;
    while(node != NULL) {
        if(node->x == x && node->y == y) {
            *value = node->value;
            return true;
        } else {
            node = node->next;
        }
    }
    return false;
}

bool tableFree(Table* table) {
    while(table->top != NULL) {
        TableNode* tmp = table->top->next;
        free(table->top);
        table->top = tmp;
    }
    free(table);
}

#endif
