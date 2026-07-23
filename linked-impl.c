#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node* nextNode;
};

struct Node* createNode(int val) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->val = val;
    newNode->nextNode = NULL;
    return newNode;
}

void addBeginningNode(struct Node** head, int val) {
    struct Node* newNode = createNode(val);
    newNode->nextNode = *head;
    *head = newNode;
}

void addEndNode(struct Node** head, int val) {
    struct Node* newNode = createNode(val);
    if (*head == NULL) {
        *head = newNode;
        return;
    };
    struct Node* temp = *head;
    while (temp->nextNode != NULL) {
        temp = temp->nextNode;
    }
    temp->nextNode = newNode;
}

void addAtPosition(struct Node** head, int val, int position) {
    struct Node* newNode = createNode(val);
    if (position == 0) {
        addBeginningNode(head, val);
        return;
    }
    struct Node* temp = *head;
    for (int i = 0; temp != NULL && i < position - 1; i++) {
        temp = temp->nextNode;
    }
    if (temp == NULL) {
        printf("postion out of bounds");
        free(newNode);
        return;
    }
    newNode->nextNode = temp->nextNode;
    temp->nextNode = newNode;
}

void deleteFirst(struct Node** head){
    if (*head == NULL) {
        printf("empty list");
        return;
    }
    struct Node* temp = *head;
    *head = temp->nextNode;
    free(temp);
}

void deleteLast(struct Node** head){
    if (*head == NULL) {
        printf("empty list");
        return;
    }
    struct Node* temp = *head;
    if (temp->nextNode == NULL) {
        free(temp);
        *head = NULL;
        return;
    }
    while (temp->nextNode->nextNode != NULL) {
        temp = temp->nextNode;
    }
    free(temp->nextNode);
    temp->nextNode = NULL;
}

void print(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->val);
        temp = temp->nextNode;
    }
    printf("NULL\n");
}

int main(){
    struct Node* head = NULL;
    addBeginningNode(&head, 10);
    print(head);

    addEndNode(&head, 20);
    print(head);

    deleteLast(&head);
    print(head);

    addEndNode(&head, 30);
    print(head);

    addAtPosition(&head, 20, 1);
    print(head);

    return 0;
}