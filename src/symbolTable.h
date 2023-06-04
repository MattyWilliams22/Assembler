#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>

// Aliases for types
typedef char* Key;
typedef uint32_t Value;

// Linked List Node
typedef struct Node{
  Key key;
  Value value;
  struct Node *next;
} Node_t;

// Linked List
typedef struct {
  Node_t *head;
} Symbol_Table;

#endif