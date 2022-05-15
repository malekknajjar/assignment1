#ifndef MyList
#define MyList

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Node
{
    void* data;
    struct Node* next;
};

struct List
{
    struct Node* head;
    struct Node* tail;
};

void ListInit(struct List* list) {
    list->head = list->tail = NULL;
}

bool ListIsEmpty(struct List* list) {
    return list->head == NULL && list->tail == NULL;
}

struct Node* createNode(void* data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void AddNodeinList(struct List* list, void* data) {
    if (list->head) {
        struct Node* newNode = createNode(data);
        list->tail->next = newNode;
        list->tail = newNode;
        list->tail->next = list->head;
    }
    else {
        list->head = list->tail = createNode(data);
        list->head->next = list->head;
    }
}

void* DeleteNodefromList(struct List* list, void* data) {
    if (!list->head)
        return NULL;
    else if (list->head->next == list->head) {
        if (data == list->head->data) {
            list->head = list->tail = NULL;
        }
        return data;
    }

    else if (list->head->data == data) {
        list->head = list->head->next;
        list->tail->next = list->head;
        return data;
    }

    struct Node* curr = list->head->next;
    struct Node* prev = list->head;
    while (curr != list->head)
    {
        if (data == curr->data) {
            prev->next = curr->next;
            if (curr == list->tail)
                list->tail = prev;
            return data;
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

void clearList(struct List* list) {
    if (ListIsEmpty(list)) 
        return;

    struct Node* curr = list->head;
    list->head = list->head->next;
    while (curr != list->tail) {
        free(curr);
        curr = list->head;
        list->head = list->head->next;
    }
    free(curr);
    list->head = list->tail = NULL;
}

#endif