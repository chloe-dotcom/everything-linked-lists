#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define INSERTIONS_PER_THREAD 10000

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
    newNode->nextNode = *head; // read shared memory
    *head = newNode;           // write shared memory
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

struct Node* head = NULL;

void* threadInsertUnsafe(void* arg) {
    for (int i = 0; i < INSERTIONS_PER_THREAD; i++) {
        addBeginningNode(&head, i);
    }
    return NULL;
}

int countNodes(struct Node* head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->nextNode;
    }
    return count;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, threadInsertUnsafe, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL); // wait for thread to terminate
    }

    int expected = NUM_THREADS * INSERTIONS_PER_THREAD;
    int actual = countNodes(head);

    printf("******TEST********\n");
    printf("Expected nodes: %d\n", expected);
    printf("Actual nodes:   %d\n", actual);
    
    if (actual != expected) {
        printf("RACE CONDITION DETECTED!\n");
    }
    return 0;
}