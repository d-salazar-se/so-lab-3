#include "lab.h"

list * createList() {
  list * l = (list*)calloc(1, sizeof(list));
  l->head = NULL;
  return l;
}

node * createNode(fileLine * call) {
  node * n = (node*)calloc(1, sizeof(node));
  n->call = (fileLine*)calloc(1, sizeof(fileLine));
  n->call = call;
  n->next = NULL;
  return n;
}

void addNodeToList(list * l, node * n) {
  if (l->head == NULL) {
    l->head = n;
    return;
  }

  node * current = l->head;

  while(current->next) {
    current = current->next;
  }

  current->next = n;
}

void addCall(list * l, fileLine * call) {
  node * n = createNode(call);
  addNodeToList(l, n);
}

int removeCallsByFloor(list * l, int floorNumber) {
  int pendingCalls = 0;
  if ( ! l->head) {
    return pendingCalls;
  }

  node * current = l->head->next;
  node * previous = l->head;
  node * temp = NULL;

  while (current && previous) {
    if (current->call->destinationFloor == floorNumber) {
      temp = current;
      previous->next = current->next;
      free(temp);
    } else {
      pendingCalls++;
    }
    previous = current;
    current = current->next;
  }

  return pendingCalls;
}
// ------------------------

void printList(list * l) {
  node * aux = l->head;
  while(aux) {
    printf("[%2d] %2d->%2d (%d)\n", aux->call->time, aux->call->originFloor, aux->call->destinationFloor, aux->call->numberOfPassengers);
    aux = aux->next;
  }
}