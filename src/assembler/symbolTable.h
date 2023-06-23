#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "assemblesetup.h"
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
  int *dependencies;
  int no_dependencies;
  struct Node *next;
} Node_t;

// Linked List
typedef struct {
  Node_t *head;
} Symbol_Table;

// Prototypes
void add_dependency(Symbol_Table *table, Key label, int line_no, token_line *lines);
void add_address(Symbol_Table *table, Key label, int line_no, token_line *lines);
void set_addresses(Node_t node, token_line *lines);
void free_table(Symbol_Table *table);
void free_list_node(Node_t *node);
Node_t *make_list_node(Key label, Value address, int no_dependencies, Node_t *next);
Node_t *find_in_table(Symbol_Table *table, Key key);

#endif