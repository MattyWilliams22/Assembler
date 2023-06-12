#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "assembleutils.h"
#include <stdint.h>

// Aliases for types
typedef char* Key;
typedef uint32_t Value;

typedef struct {
  operand operand;
  int line;
} dependency;

// Linked List Node
typedef struct Node{
  Key label;
  Value address;
  dependency *dependencies;
  int no_dependencies;
  struct Node *next;
} Node_t;

// Linked List
typedef struct {
  Node_t *head;
} Symbol_Table;

// Prototypes
void add_dependency(Symbol_Table *table, Key label, operand op, int line_no);
void add_address(Symbol_Table *table, Key label, int line_no);
void set_addresses(Symbol_Table *table, Node_t node);
void free_table(Symbol_Table *table);
void free_list_node(Node_t *node);
Node_t *make_list_node(Key label, Value address, int no_dependencies, Node_t *next);

#endif